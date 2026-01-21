/**************************************************************
 * String Library Header - BloodG OS
 **************************************************************/

#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>
#include <stdint.h>

/* ==================== STRING FUNCTIONS ==================== */

/**
 * Calculate length of a null-terminated string
 * @param str Pointer to string
 * @return Length of string (excluding null terminator)
 */
size_t strlen(const char* str);

/**
 * Compare two strings
 * @param s1 First string
 * @param s2 Second string
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int strcmp(const char* s1, const char* s2);

/**
 * Compare two strings (limited length)
 * @param s1 First string
 * @param s2 Second string
 * @param n Maximum characters to compare
 * @return 0 if equal, negative if s1 < s2, positive if s1 > s2
 */
int strncmp(const char* s1, const char* s2, size_t n);

/**
 * Copy string
 * @param dest Destination buffer
 * @param src Source string
 * @return Pointer to destination
 */
char* strcpy(char* dest, const char* src);

/**
 * Copy string with length limit
 * @param dest Destination buffer
 * @param src Source string
 * @param n Maximum characters to copy
 * @return Pointer to destination
 */
char* strncpy(char* dest, const char* src, size_t n);

/**
 * Concatenate strings
 * @param dest Destination buffer
 * @param src Source string to append
 * @return Pointer to destination
 */
char* strcat(char* dest, const char* src);

/**
 * Find first occurrence of character in string
 * @param str String to search
 * @param c Character to find
 * @return Pointer to character or NULL if not found
 */
char* strchr(const char* str, int c);

/**
 * Find last occurrence of character in string
 * @param str String to search
 * @param c Character to find
 * @return Pointer to character or NULL if not found
 */
char* strrchr(const char* str, int c);

/**
 * Locate substring
 * @param haystack String to search in
 * @param needle Substring to find
 * @return Pointer to beginning of substring or NULL
 */
char* strstr(const char* haystack, const char* needle);

/* ==================== MEMORY FUNCTIONS ==================== */

/**
 * Fill memory with constant byte
 * @param ptr Pointer to memory block
 * @param value Value to set
 * @param num Number of bytes to set
 * @return Pointer to memory block
 */
void* memset(void* ptr, int value, size_t num);

/**
 * Copy memory block
 * @param dest Destination pointer
 * @param src Source pointer
 * @param n Number of bytes to copy
 * @return Pointer to destination
 */
void* memcpy(void* dest, const void* src, size_t n);

/**
 * Move memory block (handles overlapping)
 * @param dest Destination pointer
 * @param src Source pointer
 * @param n Number of bytes to move
 * @return Pointer to destination
 */
void* memmove(void* dest, const void* src, size_t n);

/**
 * Compare memory blocks
 * @param ptr1 First memory block
 * @param ptr2 Second memory block
 * @param n Number of bytes to compare
 * @return 0 if equal, negative if ptr1 < ptr2, positive if ptr1 > ptr2
 */
int memcmp(const void* ptr1, const void* ptr2, size_t n);

/**
 * Find byte in memory block
 * @param ptr Pointer to memory block
 * @param value Value to find
 * @param n Number of bytes to search
 * @return Pointer to found byte or NULL
 */
void* memchr(const void* ptr, int value, size_t n);

/* ==================== CHARACTER FUNCTIONS ==================== */

/**
 * Check if character is alphabetic
 * @param c Character to check
 * @return Non-zero if alphabetic, 0 otherwise
 */
int isalpha(int c);

/**
 * Check if character is digit
 * @param c Character to check
 * @return Non-zero if digit, 0 otherwise
 */
int isdigit(int c);

/**
 * Check if character is alphanumeric
 * @param c Character to check
 * @return Non-zero if alphanumeric, 0 otherwise
 */
int isalnum(int c);

/**
 * Check if character is whitespace
 * @param c Character to check
 * @return Non-zero if whitespace, 0 otherwise
 */
int isspace(int c);

/**
 * Convert character to uppercase
 * @param c Character to convert
 * @return Uppercase equivalent
 */
int toupper(int c);

/**
 * Convert character to lowercase
 * @param c Character to convert
 * @return Lowercase equivalent
 */
int tolower(int c);

/* ==================== CONVERSION FUNCTIONS ==================== */

/**
 * Convert string to integer
 * @param str String to convert
 * @return Integer value
 */
int atoi(const char* str);

/**
 * Convert integer to string
 * @param value Integer value
 * @param str Buffer to store result
 * @param base Number base (2-36)
 * @return Pointer to string
 */
char* itoa(int value, char* str, int base);

/**
 * Convert unsigned integer to string
 * @param value Unsigned integer value
 * @param str Buffer to store result
 * @param base Number base (2-36)
 * @return Pointer to string
 */
char* utoa(uint32_t value, char* str, int base);

/**
 * Convert integer to hexadecimal string
 * @param value Integer value
 * @param str Buffer to store result
 * @return Pointer to string
 */
char* itox(uint32_t value, char* str);

#endif // _STRING_H
