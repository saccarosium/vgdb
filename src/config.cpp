#include "config.h"

namespace config {

static std::filesystem::path get_path()
{
    std::string config_path;
    const char* xdg_config_env = getenv("XDG_CONFIG_HOME");
    if (xdg_config_env != NULL) {
        config_path = xdg_config_env;
    } else {
        const char* home = getenv("HOME");
        config_path = std::format("{}/.config", home);
    }

    return std::format("{}/tug/config.ini", config_path);
}

static std::string read()
{
    std::filesystem::path config_path(get_path());
    if (!std::filesystem::exists(config_path) || !std::filesystem::is_regular_file(config_path))
        return default_ini;

    std::ifstream file(config_path, std::ios::binary | std::ios::ate);
    if (!file)
        return default_ini;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(static_cast<std::size_t>(size), '\0');
    if (!file.read(buffer.data(), size))
        return default_ini;

    return buffer;
}

void load(GUI& gui)
{
    std::string ini_data = read();
    size_t ini_data_end_idx = ini_data.find("; ImGui Begin");
    if (ini_data_end_idx == SIZE_MAX)
        ini_data_end_idx = ini_data.size();

    const auto LoadString = [&](String key, String default_value) -> String {
        String result;
        key += "=";
        size_t index = ini_data.find(key);
        if (index < ini_data_end_idx) {
            size_t start_index = index + key.size();
            size_t end_index = start_index;
            while (end_index < ini_data_end_idx) {
                char c = ini_data[end_index];
                if (c == '\n' || c == '\r') {
                    break;
                } else {
                    end_index++;
                }
            }

            result = ini_data.substr(start_index, end_index - start_index);
        } else {
            result = default_value;
        }

        return result;
    };

    const auto LoadFloat = [&](String key, float default_value) -> float {
        float result = default_value;
        String str_value = LoadString(key, "");
        float x = strtof(str_value.c_str(), NULL);
        if (x != 0.0f || (str_value.size() != 0 && str_value[0] == '0')) {
            result = x;
        }
        return result;
    };

    const auto LoadBool = [&](String key, bool default_value) -> bool {
        return ("false" != LoadString(key, default_value ? "true" : "false"));
    };

    gui.show_callstack = LoadBool("Callstack", true);
    gui.show_locals = LoadBool("Locals", true);
    gui.show_watch = LoadBool("Watch", true);
    gui.show_control = LoadBool("Control", true);
    gui.show_breakpoints = LoadBool("Breakpoints", true);
    gui.show_source = LoadBool("Source", true);
    gui.show_registers = LoadBool("Registers", false);
    gui.show_threads = LoadBool("Threads", false);
    gui.show_directory_viewer = LoadBool("DirectoryViewer", true);

    float font_size = LoadFloat("FontSize", DEFAULT_FONT_SIZE);
    if (font_size != 0.0f) {
        gui.font_size = font_size;
        gui.source_font_size = font_size;
    }

    gui.font_filename = LoadString("FontFilename", "");
    if (gui.font_filename != "") {
        gui.change_font = true;
        gui.use_default_font = false;
    }

    String theme = LoadString("WindowTheme", "DarkBlue");
    gui.window_theme = (theme == "Light") ? WindowTheme_Light
        : (theme == "DarkPurple")         ? WindowTheme_DarkPurple
                                          : WindowTheme_DarkBlue;
    gui.hover_delay_ms = (int)LoadFloat("HoverDelay", 100);

    // load debug session history
    int session_idx = 0;
    while (true) {
        String exef = StringPrintf("DebugFilename%d", session_idx);
        String argf = StringPrintf("DebugArgs%d", session_idx);
        session_idx += 1;

        Session s = {};
        s.debug_exe = LoadString(exef, "");
        s.debug_args = LoadString(argf, "");

        if (s.debug_exe.size() <= 0)
            break;

        gui.session_history.push_back(s);
    }

    if (ini_data.size() != 0) {
        ImGui::LoadIniSettingsFromMemory(ini_data.data(), ini_data.size());
    }
}

void save(const GUI& gui)
{
    std::filesystem::path config_path(get_path());
    std::filesystem::path config_dirname(config_path.parent_path());
    if (!std::filesystem::exists(config_dirname))
        std::filesystem::create_directories(config_dirname);

    std::ofstream file(config_path, std::ios::binary | std::ios::app);
    if (!file)
        return;

    // write custom tug ini information
    file << "[Tug]\n";

    // save docking tab visibility, imgui doesn't save this at the moment
    file << std::format("Callstack={}\n", (int)gui.show_callstack);
    file << std::format("Locals={}\n", (int)gui.show_locals);
    file << std::format("Registers={}\n", (int)gui.show_registers);
    file << std::format("Watch={}\n", (int)gui.show_watch);
    file << std::format("Control={}\n", (int)gui.show_control);
    file << std::format("Source={}\n", (int)gui.show_source);
    file << std::format("Breakpoints={}\n", (int)gui.show_breakpoints);
    file << std::format("Threads={}\n", (int)gui.show_threads);
    file << std::format("DirectoryViewer={}\n", gui.show_directory_viewer);
    file << std::format("FontFilename={}\n", gui.font_filename.c_str());
    file << std::format("FontSize={}\n", gui.font_size);

    String theme = (gui.window_theme == WindowTheme_Light) ? "Light"
        : (gui.window_theme == WindowTheme_DarkPurple)     ? "DarkPurple"
                                                           : "DarkBlue";
    file << std::format("WindowTheme={}\n", theme.c_str());

    file << std::format("HoverDelay={}\n", gui.hover_delay_ms);
    file << std::format("CursorBlink={}\n", ImGui::GetIO().ConfigInputTextCursorBlink);

    // write the imgui side of the ini file
    size_t imgui_ini_size = 0;
    const char* imgui_ini_data = ImGui::SaveIniSettingsToMemory(&imgui_ini_size);
    if (imgui_ini_data && imgui_ini_size) {
        file << "\n; ImGui Begin\n";
        file.write(imgui_ini_data, imgui_ini_size);
    }
}

}