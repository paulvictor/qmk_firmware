#include QMK_KEYBOARD_H

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.

#define ALT_X LALT(KC_X)
#define STUMP_PREFIX_MOD (MOD_LALT | MOD_LCTL | MOD_LGUI)

enum ferris_layers {
  _BASE,
  _NUM,
  _SYMB,
  _MOUSE,
  _ARROWS,
  _FUNCTIONS
};

//Tap dance enums
enum tap_dances {
  CTL_META_X = 0,
  MAX_TAP_DANCE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT(
    KC_MINUS, KC_W, LT(_FUNCTIONS,KC_F), KC_P, KC_B,                      KC_Q, KC_U, KC_Y, KC_Z, KC_QUOTE,
    MT(STUMP_PREFIX_MOD, KC_A), MT(MOD_LALT, KC_R), LT(_ARROWS,KC_S), MT(MOD_LSFT, KC_T),KC_G,      KC_M, MT(MOD_RSFT, KC_N), KC_E, MT(MOD_LALT, KC_I), MT(MOD_RCTL, KC_O),
    TD(CTL_META_X), KC_X, KC_C, LT(_MOUSE,KC_D), KC_V,         KC_DOT, KC_H, KC_J, KC_K, KC_L,
        LT(_NUM,KC_SPACE), LT(_SYMB,KC_TAB),      MT(MOD_RSFT, KC_ENTER),KC_BSPC
  ),

  [_NUM] = LAYOUT(
    _______, KC_SLASH, KC_BSLS, KC_PIPE, _______,              KC_TILD,  KC_4, KC_5, KC_6, _______,
    KC_LABK, KC_LPRN, KC_RPRN,  KC_RABK, _______,              KC_GRAVE, KC_1, KC_2, KC_3, KC_PLUS,
    KC_LCBR, KC_LBRC,  KC_RBRC, KC_RCBR, _______,              _______,  KC_7, KC_8, KC_9, KC_0,
                                     _______, _______, _______, _______
  ),

  [_SYMB] = LAYOUT(
    _______, _______, _______, _______, _______,                       _______, KC_EXLM, KC_AT,    KC_HASH, _______,
    _______, _______, _______, _______, _______,                       _______, KC_DLR,  KC_PERC,  KC_CIRC, KC_EQUAL,
    _______, _______, _______, _______, _______,                       _______, KC_AMPR, KC_ASTR,  KC_QUES, _______,
                                  _______, _______, _______, _______
  ),
  [_MOUSE] = LAYOUT(
    _______, _______, _______, _______, _______,                       _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,                       _______, KC_MS_LEFT, KC_MS_DOWN, KC_MS_UP, KC_MS_RIGHT,
    _______, _______, _______, _______, _______,                     _______, KC_MS_WH_LEFT, KC_MS_WH_DOWN, KC_MS_WH_UP, KC_MS_WH_RIGHT,
                                  _______, _______, KC_MS_BTN1, KC_MS_BTN2
  ),
  [_ARROWS] = LAYOUT(
    _______, _______, _______, _______, _______,                       _______, _______, _______, _______,  _______,
    _______, _______, _______, _______, _______,                       _______, KC_LEFT,  KC_DOWN, KC_UP, KC_RIGHT,
    _______, _______, _______, _______, _______,                       _______, KC_LEFT,  KC_DOWN, KC_UP, KC_RIGHT,
                                 _______, _______, _______, _______
  ),
  [_FUNCTIONS] = LAYOUT(
    _______, _______, _______, _______, _______,                      _______, KC_F4, KC_F5, KC_F6, KC_F12,
    _______, _______, _______, _______, _______,                      _______, KC_F1, KC_F2, KC_F3, KC_F11,
    _______, _______, _______, _______, _______,                      _______, KC_F7, KC_F8, KC_F9, KC_F10,
                                 _______, _______, _______, _______
  )
};

const uint16_t PROGMEM esc_1[] = { MT(MOD_RSFT, KC_N), KC_E, COMBO_END };
const uint16_t PROGMEM esc_2[] = { KC_E, MT(MOD_LALT, KC_I), COMBO_END };
const uint16_t PROGMEM colon_combo[] = { LT(_NUM,KC_SPACE),KC_BSPC, COMBO_END };
const uint16_t PROGMEM semicolon_combo[] = { LT(_SYMB,KC_TAB), MT(MOD_RSFT, KC_ENTER), COMBO_END };

combo_t key_combos[] = {
    COMBO(esc_1, KC_ESCAPE),
    COMBO(esc_2, KC_ESCAPE),
    COMBO(colon_combo, KC_COLN),
    COMBO(semicolon_combo, KC_SCLN),
};

const key_override_t delete_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);
const key_override_t dot_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_DOT, KC_COMMA);

// This globally defines all key overrides to be used
const key_override_t **key_overrides = (const key_override_t *[]){
	&delete_key_override,
  &dot_key_override,
	NULL // Null terminate the array of overrides!
};


// For tap dance

typedef struct {
    bool is_press_action;
    uint8_t step;
} tap;

enum {
    SINGLE_TAP = 1,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    DOUBLE_SINGLE_TAP,
    MORE_TAPS
};

static tap dance_state[MAX_TAP_DANCE];

uint8_t dance_step(tap_dance_state_t *state);

uint8_t dance_step(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return SINGLE_TAP;
        else return SINGLE_HOLD;
    } else if (state->count == 2) {
        if (state->interrupted) return DOUBLE_SINGLE_TAP;
        else if (state->pressed) return DOUBLE_HOLD;
        else return DOUBLE_TAP;
    }
    return MORE_TAPS;
}


void cmx_finished(tap_dance_state_t *state, void *user_data);
void cmx_reset(tap_dance_state_t *state, void *user_data);

void cmx_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[0].step = dance_step(state);
    switch (dance_state[0].step) {
        case SINGLE_TAP: register_code16(LALT(KC_X)); break;
        case SINGLE_HOLD: register_code16(KC_LEFT_CTRL); break;
        case DOUBLE_TAP: register_code16(LCTL(KC_X)); break;
        case DOUBLE_SINGLE_TAP: tap_code16(LALT(KC_X)); register_code16(LALT(KC_X));
    }
}

void cmx_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[0].step) {
        case SINGLE_TAP: unregister_code16(LALT(KC_X)); break;
        case SINGLE_HOLD: unregister_code16(KC_LEFT_CTRL); break;
        case DOUBLE_TAP: unregister_code16(LCTL(KC_X)); break;
        case DOUBLE_SINGLE_TAP: unregister_code16(LALT(KC_X)); break;
    }
    dance_state[0].step = 0;
}


tap_dance_action_t tap_dance_actions[] = {
  [CTL_META_X] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, cmx_finished, cmx_reset)
};
