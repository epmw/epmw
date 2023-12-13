#ifndef UI_SELECT_FROM_OPTIONS_SCREEN
#define UI_SELECT_FROM_OPTIONS_SCREEN

#include <stdint.h>

#define UI_SELECT_FROM_OPTIONS_SCREEN_FAILURE_RETURN 255

typedef struct{
	const uint8_t count_of_options;
	const char **options;
} ui_string_options_t;

typedef struct{
	const uint8_t count_of_options;
	const uint8_t *options;
} ui_uint8_t_options_t;

typedef enum ui_option_type{
	UI_OPTION_TYPE_STRING,
	UI_OPTION_TYPE_UINT8_T,
} ui_option_type_t;

uint8_t ui_select_from_options_screen(const ui_option_type_t options_type, const void *options_struct, const char *screen_text);

#endif
