#include QMK_KEYBOARD_H

#define ALT_X LALT(KC_X)
#define STUMP_PREFIX_MOD (MOD_LGUI)

enum ferris_layers {
  _BASE,
  _NUM,
  _SYMB,
  _MOUSE,
  _FUNCTIONS
};

//Tap dance enums

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

enum tap_dances {
  CTL_META_X = 0,
/*   SUPER_CTL_X, */
  MAX_TAP_DANCE
};

enum custom_keycodes {
  CAPSWORD_ON = SAFE_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT(
    KC_MINUS, KC_Y, LT(_FUNCTIONS,KC_Q), KC_U, KC_W,                         KC_Z, KC_P, KC_B, KC_F, KC_QUOTE,
    LGUI_T(KC_A), MT(MOD_LALT, KC_R), KC_S, MT(MOD_LSFT, KC_T),KC_G,         KC_M, MT(MOD_RSFT, KC_N), KC_E, MT(MOD_LALT, KC_I), MT(MOD_RCTL, KC_O),
    TD(CTL_META_X), KC_X, KC_C, LT(_MOUSE,KC_D), KC_V,                       KC_DOT, KC_H, KC_J, KC_K, KC_L,
                     LT(_NUM,KC_SPACE), LT(_SYMB,KC_TAB),      LCA_T(KC_ENTER), KC_BSPC
  ),

  [_NUM] = LAYOUT(
    _______, KC_SLASH, KC_BSLS, KC_PIPE, _______,              KC_GRAVE, KC_4, KC_5, KC_6, KC_PLUS,
    KC_LABK, KC_LPRN , KC_RPRN, KC_RABK, _______,              KC_QUES,  KC_1, KC_2, KC_3,    KC_0,
    KC_LCBR, KC_LBRC , KC_RBRC, KC_RCBR, _______,              _______,  KC_7, KC_8, KC_9, KC_EQUAL,
                                     _______, _______, _______, _______
  ),

  [_SYMB] = LAYOUT(
    _______, _______, _______, _______, _______,               KC_TILD, KC_DLR, KC_PERC, KC_CIRC, _______,
    _______, _______, _______, _______, _______,               KC_AMPR, KC_EXLM, KC_AT,   KC_HASH, KC_ASTR,
    _______, _______, _______, _______, _______,               _______, KC_LEFT, KC_DOWN, KC_UP  , KC_RIGHT,
                                  _______, _______, _______, _______
  ),
  [_MOUSE] = LAYOUT(
    _______, _______, _______, _______, _______,               _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______,               _______, KC_MS_LEFT, KC_MS_DOWN, KC_MS_UP, KC_MS_RIGHT,
    _______, _______, _______, _______, _______,               _______, KC_MS_WH_LEFT, KC_MS_WH_UP, KC_MS_WH_DOWN, KC_MS_WH_RIGHT,
                                  _______, _______, KC_MS_BTN1, KC_MS_BTN2
  ),
  [_FUNCTIONS] = LAYOUT(
    _______, _______, _______, _______, _______,               _______, KC_F4, KC_F5, KC_F6, KC_F12,
    _______, _______, _______, _______, _______,               _______, KC_F1, KC_F2, KC_F3, KC_F11,
    _______, _______, _______, _______, _______,               _______, KC_F7, KC_F8, KC_F9, KC_F10,
                                 _______, _______, _______, _______
  )
};

const uint16_t PROGMEM esc_1[] = { MT(MOD_RSFT, KC_N), KC_E, COMBO_END };
const uint16_t PROGMEM colon_combo[] = { LT(_NUM,KC_SPACE), KC_BSPC, COMBO_END };
const uint16_t PROGMEM semicolon_combo[] = { LT(_SYMB,KC_TAB), LCA_T(KC_ENTER), COMBO_END };
const uint16_t PROGMEM caps_combo[] = { KC_H, KC_J, COMBO_END };

combo_t key_combos[] = {
    COMBO(esc_1, KC_ESCAPE),
    COMBO(colon_combo, KC_COLN),
    COMBO(semicolon_combo, KC_SCLN),
    COMBO(caps_combo, CAPSWORD_ON),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case CAPSWORD_ON:
      if (record->event.pressed) {
        caps_word_on();
        return false;
      }
  }
  return true;
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

td_state_t cur_dance(tap_dance_state_t *state);

void cmx_finished(tap_dance_state_t *state, void *user_data);
void cmx_reset(tap_dance_state_t *state, void *user_data);

/* Return an integer that corresponds to what kind of tap dance should be executed.
 *
 * How to figure out tap dance state: interrupted and pressed.
 *
 * Interrupted: If the state of a dance is "interrupted", that means that another key has been hit
 *  under the tapping term. This is typically indicitive that you are trying to "tap" the key.
 *
 * Pressed: Whether or not the key is still being pressed. If this value is true, that means the tapping term
 *  has ended, but the key is still being pressed down. This generally means the key is being "held".
 *
 * One thing that is currenlty not possible with qmk software in regards to tap dance is to mimic the "permissive hold"
 *  feature. In general, advanced tap dances do not work well if they are used with commonly typed letters.
 *  For example "A". Tap dances are best used on non-letter keys that are not hit while typing letters.
 *
 * Good places to put an advanced tap dance:
 *  z,q,x,j,k,v,b, any function key, home/end, comma, semi-colon
 *
 * Criteria for "good placement" of a tap dance key:
 *  Not a key that is hit frequently in a sentence
 *  Not a key that is used frequently to double tap, for example 'tab' is often double tapped in a terminal, or
 *    in a web form. So 'tab' would be a poor choice for a tap dance.
 *  Letters used in common words as a double. For example 'p' in 'pepper'. If a tap dance function existed on the
 *    letter 'p', the word 'pepper' would be quite frustating to type.
 *
 * For the third point, there does exist the 'TD_DOUBLE_SINGLE_TAP', however this is not fully tested
 *
 */
td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }
    // Exit if none of these
    return TD_UNKNOWN;

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

static td_state_t cmx_td_state;
static td_state_t sca_td_state;

void cmx_finished(tap_dance_state_t *state, void *user_data) {
  cmx_td_state = cur_dance(state);
  switch (cmx_td_state) {
  case TD_SINGLE_TAP: register_code16(LCTL(KC_X)); break;
  case TD_SINGLE_HOLD: register_code16(KC_LCTL); break;
  case TD_DOUBLE_TAP: register_code16(LALT(KC_X)); break;
  case TD_DOUBLE_HOLD: tap_code16(LCTL(KC_X));register_code16(KC_LCTL); break;
    // Last case is for fast typing. Assuming your key is `f`:
    // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
    // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
  case TD_DOUBLE_SINGLE_TAP: register_code16(LALT(KC_X)); break;
  default: break;
  }
}

void cmx_reset(tap_dance_state_t *state, void *user_data) {
  switch (cmx_td_state) {
  case TD_SINGLE_TAP: unregister_code16(LCTL(KC_X)); break;
  case TD_SINGLE_HOLD: unregister_code16(KC_LCTL); break;
  case TD_DOUBLE_TAP: unregister_code16(LALT_T(KC_X)); break;
  case TD_DOUBLE_HOLD: unregister_code16(KC_LCTL); break;
  case TD_DOUBLE_SINGLE_TAP: unregister_code16(LALT(KC_X)); break;
  default: break;
  }
  cmx_td_state = TD_NONE;
}

void sca_finished(tap_dance_state_t *state, void *user_data) {
  sca_td_state = cur_dance(state);
  switch (sca_td_state) {
  case TD_SINGLE_TAP: register_code16(KC_A); break;
  case TD_SINGLE_HOLD: register_code16(KC_LGUI); break;
  case TD_DOUBLE_TAP: tap_code16(KC_A);register_code16(KC_A); break;
  case TD_DOUBLE_HOLD: register_code16(KC_LCTL); break;
  case TD_DOUBLE_SINGLE_TAP: tap_code16(KC_A);register_code16(KC_A); break;
  default: break;
  }
}

void sca_reset(tap_dance_state_t *state, void *user_data) {
  switch (sca_td_state) {
  case TD_SINGLE_TAP: unregister_code16(KC_A); break;
  case TD_SINGLE_HOLD: unregister_code16(KC_LGUI); break;
  case TD_DOUBLE_HOLD: unregister_code16(KC_LCTL); break;
  case TD_DOUBLE_SINGLE_TAP: unregister_code16(KC_A); break;
  case TD_DOUBLE_TAP: unregister_code16(KC_A); break;
  default: break;
  }
  sca_td_state = TD_NONE;
}

tap_dance_action_t tap_dance_actions[] = {
  [CTL_META_X] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, cmx_finished, cmx_reset)
/*   , [SUPER_CTL_X] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, sca_finished, sca_reset) */
};
