#ifndef GNF_TEXT_INPUT_HEADER
#define GNF_TEXT_INPUT_HEADER

typedef struct {
    std::string input;
} textInputData;


bool gnf_text_input_box(gnfContext *gnf, textInputData *textinput, Vec2 p, Vec2 s) {
    if (gnf->pressed_character) {
        textinput->input.append(&(gnf->pressed_character));
        gnf->pressed_character = 0;
    }

    if (gnf->pressed_key == GNF_KEY_BACKSPACE) {
        if (textinput->input.size() > 0) {
            textinput->input.pop_back();
        }
        gnf->pressed_key = GNF_KEY_NONE;
    }

    if (gnf->pressed_key == GNF_KEY_ENTER) {
        gnf->pressed_key = GNF_KEY_NONE;
        return true;
    }

    gnf_fill_rect(gnf, p, s, GNF_WHITE);
    gnf_render_text(gnf, p, GNF_PACKAGE_HEADER_SCALE, GNF_BLACK, textinput->input.c_str());
    return false;
}


#endif // GNF_TEXT_INPUT_HEADER
