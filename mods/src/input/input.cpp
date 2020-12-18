#include <vector>

#include <libcore/libcore.h>

#include "../feature/feature.h"
#include "input.h"

#include "../minecraft.h"

// Open Sign Screen
static void LocalPlayer_openTextEdit_injection(unsigned char *local_player, unsigned char *sign) {
    if (*(int32_t *) (sign + TileEntity_id_property_offset) == 4) {
        unsigned char *minecraft = *(unsigned char **) (local_player + LocalPlayer_minecraft_property_offset);
        unsigned char *screen = (unsigned char *) ::operator new(TEXT_EDIT_SCREEN_SIZE);
        screen = (*TextEditScreen)(screen, sign);
        (*Minecraft_setScreen)(minecraft, screen);
    }
}

#define BACKSPACE_KEY 8

static int is_valid_key(char key) {
    return (key >= 32 && key <= 126) || key == BACKSPACE_KEY;
}

// Store Text Input
std::vector<char> input;
void input_key_press(char key) {
    if (is_valid_key(key)) {
        input.push_back(key);
    }
}
void input_clear_input() {
    input.clear();
}

// Handle Text Input
static void TextEditScreen_updateEvents_injection(unsigned char *screen) {
    // Call Original Method
    (*Screen_updateEvents)(screen);

    if (*(char *)(screen + 4) == '\0') {
        uint32_t vtable = *((uint32_t *) screen);
        for (char key : input) {
            if (key == BACKSPACE_KEY) {
                // Handle Backspace
                (*(Screen_keyPressed_t *) (vtable + Screen_keyPressed_vtable_offset))(screen, BACKSPACE_KEY);
            } else {
                // Handle Nrmal Key
                (*(Screen_keyboardNewChar_t *) (vtable + Screen_keyboardNewChar_vtable_offset))(screen, key);
            }
        }
    }
    input_clear_input();
}

void init_input_cpp() {
    if (feature_has("Fix Sign Placement")) {
        // Fix Signs
        patch_address(LocalPlayer_openTextEdit_vtable_addr, (void *) LocalPlayer_openTextEdit_injection);
        patch_address(TextEditScreen_updateEvents_vtable_addr, (void *) TextEditScreen_updateEvents_injection);
    }
}