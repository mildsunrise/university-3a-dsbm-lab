/****************************************************

 b a s e c o n v e r t . c

 Custom program: Base converter

 *****************************************************/

#include "Base.h"     // Basic definitions
#include "util.h"     // Generic utilities
#include "lcd.h"      // LCD module header file
#include "keyboard.h" // 4x4 keyboard module header file
#include "encoder.h"  // Quadrature encoder module header file

#include "baseconvert.h"


/** BASIC USER INPUT LAYER **/

int32_t enterString(char *string, int32_t len, int32_t maxLength, InputController controller, void* arg, int32_t col, int32_t row, int32_t *encoderBreak) {
    // Enable cursor
    LCD_GotoXY(col + len, row);
    LCD_Config(TRUE, TRUE, FALSE);

    // Input loop
    int32_t incomplete_key, incomplete = 0;
    int32_t lastEncoder = encoderCount;
    while (1) {
        // Delay (to prevent chattering) and wait for key released
        DELAY_US(2000);
        while (readKeyboard() != KEY_NOT_FOUND);
        DELAY_US(2000);

        // Read key (TODO: timeout for incomplete key)
        int32_t key;
        do {
            key = readKeyboard();
            if (encoderBreak && (lastEncoder != encoderCount)) {
                *encoderBreak = 1;
                break;
            }
        } while (key == KEY_NOT_FOUND);

        // Before reacting, finish incomplete key if any
        if (incomplete > 0 && key != incomplete_key) {
            LCD_Config(TRUE, TRUE, FALSE);
            LCD_SendChar(string[len]);
            len++;
            incomplete = 0;
        }

        if (encoderBreak && *encoderBreak) break;

        // React to key
        int32_t ch = controller(string, len, key, incomplete, arg);
        if (ch == -2) {
            // Before returning, finish incomplete key if any
            if (incomplete > 0 && key != incomplete_key) {
                LCD_Config(TRUE, TRUE, FALSE);
                LCD_SendChar(string[len]);
                len++;
                incomplete = 0;
            }
            break;
        } else if (ch == -3) {
            // Before deleting, finish incomplete key if any
            if (incomplete > 0 && key != incomplete_key) {
                LCD_Config(TRUE, TRUE, FALSE);
                LCD_SendChar(string[len]);
                len++;
                incomplete = 0;
            }
            if (len > 0) {
                len--;
                LCD_GotoXY(col + len, row);
                LCD_SendChar(' ');
                LCD_GotoXY(col + len, row);
            }
        } else if (ch > 0) {
            if (len < maxLength) {
                // Write char to string
                string[len] = ch;
                LCD_SendChar(ch & 0xFF);
                len++;
                // Start (or continue) incomplete char
                if ((ch & INPUT_CONTROLLER_MULTIPLE) != 0) {
                    len--;
                    LCD_GotoXY(col + len, row);
                    LCD_Config(TRUE, FALSE, TRUE);
                    incomplete_key = key;
                    incomplete++;
                } else if (incomplete > 0) {
                    // Finish incomplete char
                    LCD_Config(TRUE, TRUE, FALSE);
                    LCD_SendChar(string[len]);
                    len++;
                    incomplete = 0;
                }
            }
        }
    }

    // Disable cursor and blink
    LCD_Config(TRUE, FALSE, FALSE);

    // Terminate string and return length
    string[len] = '\0';
    return len;
}

int32_t numberInputController(char *string, int32_t len, int32_t key, int32_t var, void *arg) {
    int32_t radix = *((int32_t*)arg);
    char ch = KEY_CHARS[key];

    // Handle first the case of D (incomplete) when radix > 14
    if (ch == 'D' && radix > 14) {
        // Return wraparound D-E-F-etc. depending on radix
        ch += var % (radix - 13);
        return INPUT_CONTROLLER_MULTIPLE | ch;
    }

    // Handle regular number / letter presses
    if (ch >= '0' && ch <= '9' && (ch - '0') < radix) return ch;
    if (ch >= 'A' && ch <= 'D' && (ch - 'A' + 0xA) < radix) return ch;

    // Other key presses
    if (ch == '*') return -3;
    if (ch == '#') return -2;
    return -1;
}

static inline int32_t enterNumber(char *string, int32_t *len, int32_t maxLength, int32_t radix, int32_t col, int32_t row, int32_t *encoderBreak) {
    *len = enterString(string, *len, maxLength, numberInputController, &radix, col, row, encoderBreak);
    return (*len > 0) ? atoi(string, radix) : -1;
}


/** PROGRAM **/

typedef struct {
    int32_t radix;
    int32_t maxLength;
    char line [LCD_COLUMNS+1];
    char string [32]; // at worst case has to fit 31 binary digits + NUL
    int32_t len;
    int32_t overflow;
} ConversionBucket;

static void setBucket(ConversionBucket *bucket, int32_t n);

static void initBucket(ConversionBucket *bucket, int32_t n, int32_t row) {
    int32_t col;

    // Clear row
    LCD_GotoXY(0, row);
    for (col = 0; col < LCD_COLUMNS; col++)
        LCD_SendChar(' ');

    // Input base
    LCD_GotoXY(0, row);
    LCD_SendString("Base:");

    char string [3];
    int32_t len = 0, radix = -1;
    while (!(radix >= 2 && radix <= 36))
        radix = enterNumber(string, &len, 2, 10, 6, row, NULL);

    // Calculate maximum length for source base input
    // length can not be longer than floor( 31 / log2(base) )
    uint32_t maxLength = 0, current = 1;
    while (TRUE) {
        uint32_t next = current * (uint32_t)radix;
        if (next / (uint32_t)radix != current || (current & BIT31)) break;
        current = next;
        maxLength++;
    }

    // Set bucket parameters
    bucket->radix = radix;
    bucket->maxLength = maxLength;

    // Render static line part
    col = 0;
    bucket->line[col++] = 'b';
    col += strlen(itoa(radix, &bucket->line[col], 10));
    while (col < 3)
        bucket->line[col++] = ' ';
    bucket->line[col++] = 2;
    while (col < LCD_COLUMNS)
        bucket->line[col++] = ' ';
    bucket->line[LCD_COLUMNS] = 0;

    // Set bucket number
    setBucket(bucket, n);
}

static void setBucket(ConversionBucket *bucket, int32_t n) {
    // If no number provided, empty string
    if (n == -1) {
        bucket->string[0] = 0;
        bucket->len = 0;
        bucket->overflow = 0;
        return;
    }

    // Convert n to string at our radix
    itoa(n, bucket->string, bucket->radix);
    bucket->len = strlen(bucket->string);
    bucket->overflow = 0;

    // If string too long, put ellipsis at the end
    if (bucket->len > (LCD_COLUMNS - 4)) {
        bucket->len = LCD_COLUMNS - 4 - 1;
        bucket->string[bucket->len] = 0;
        bucket->overflow = 1;
    }
}

static void renderBucket(ConversionBucket *bucket, int32_t row) {
    LCD_GotoXY(0, row);
    LCD_SendString(bucket->line);
    LCD_GotoXY(4, row);
    LCD_SendString(bucket->string);
    if (bucket->overflow) {
        LCD_GotoXY(LCD_COLUMNS-1, row);
        LCD_SendChar(3);
    }
}

static int32_t promptBucket(ConversionBucket *bucket, int32_t row, int32_t *encoderBreak) {
    return enterNumber(bucket->string, &bucket->len, bucket->maxLength, bucket->radix, 4, row, encoderBreak);
}

void baseConverter(void) {
    // Initialize LCD
    LCD_Config(TRUE, FALSE, FALSE);
    static const uint8_t separator [] = {
        0b00000000,
        0b00000100,
        0b00000110,
        0b00011111,
        0b00000110,
        0b00000100,
        0b00000000,
        0b00000000,
    };
    LCD_CustomChar(2, separator);
    static const uint8_t ellipsis [] = {
        0b00000000,
        0b00010000,
        0b00011100,
        0b00011111,
        0b00011100,
        0b00010000,
        0b00000000,
        0b00000000,
    };
    LCD_CustomChar(3, ellipsis);

    // Initialize (ask for base) on both buckets
    int32_t n = -1;
    static ConversionBucket buckets [2];
    initBucket(&buckets[0], n, 0);
    renderBucket(&buckets[0], 0);
    initBucket(&buckets[1], n, 1);
    renderBucket(&buckets[1], 1);

    // Repeatedly ask on one bucket, set value
    int32_t lastEncoder = encoderCount;
    int32_t sel = 0;
    while (1) {
        int32_t encoderBreak = 0;
        int32_t input = promptBucket(&buckets[sel], sel, &encoderBreak);
        if (encoderBreak) {
            int32_t encoderDiff = encoderCount - lastEncoder;
            lastEncoder += encoderDiff;
            sel += encoderDiff;
            if (sel < 0) sel = 0;
            if (sel > 1) sel = 1;
            continue;
        }
        if (input == -1) {
            initBucket(&buckets[sel], n, sel);
            renderBucket(&buckets[sel], sel);
        } else {
            n = input;
            int32_t i;
            for (i = 0; i < 2; i++) {
                setBucket(&buckets[i], n);
                renderBucket(&buckets[i], i);
            }
        }
    }
}
