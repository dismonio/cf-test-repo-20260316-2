/*
 * IRAM Diet Override Linker Script
 *
 * Moves select libc.a time functions from IRAM to flash to reclaim ~4-8KB.
 * These functions are not called from ISRs or during flash operations.
 *
 * INSERT AFTER .flash.text places these in flash alongside normal code.
 * Because sections are consumed first-match, the .iram0.text wildcards
 * in sections.ld won't re-claim what we already placed here.
 */

SECTIONS
{
  .flash_iram_override :
  {
    /* strftime/wcsftime: ~3,266 bytes - date/time formatting */
    *libc.a:lib_a-strftime.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-wcsftime.*(.literal .literal.* .text .text.*)
    /* mktime: ~1,435 bytes - time conversion */
    *libc.a:lib_a-mktime.*(.literal .literal.* .text .text.*)
    /* strptime: time parsing */
    *libc.a:lib_a-strptime.*(.literal .literal.* .text .text.*)
    /* localtime/gmtime: timezone conversions */
    *libc.a:lib_a-lcltime_r.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-lcltime.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-gmtime_r.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-gmtime.*(.literal .literal.* .text .text.*)
    /* tzset: timezone setup */
    *libc.a:lib_a-tzset_r.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-tzset.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-tzcalc_limits.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-tzvars.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-tzlock.*(.literal .literal.* .text .text.*)
    /* time/timelocal: basic time functions */
    *libc.a:lib_a-time.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-timelocal.*(.literal .literal.* .text .text.*)
    /* ctime/asctime: time string conversion */
    *libc.a:lib_a-ctime.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-ctime_r.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-asctime.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-asctime_r.*(.literal .literal.* .text .text.*)
    /* month_lengths/gettzinfo: calendar/timezone data */
    *libc.a:lib_a-month_lengths.*(.literal .literal.* .text .text.*)
    *libc.a:lib_a-gettzinfo.*(.literal .literal.* .text .text.*)
  }
} INSERT AFTER .flash.text
