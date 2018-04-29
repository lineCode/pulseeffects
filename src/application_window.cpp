#include <iostream>
#include "application_window.hpp"
#include "util.hpp"

ApplicationWindow::ApplicationWindow(Application* application)
    : app(application),
      builder(Gtk::Builder::create_from_resource(
          "/com/github/wwmm/pulseeffects/application.glade")),
      settings(app->settings) {
    apply_css_style("listbox.css");

    Gtk::IconTheme::get_default()->add_resource_path(
        "/com/github/wwmm/pulseeffects/");

    builder->get_widget("ApplicationWindow", window);
    builder->get_widget("theme_switch", theme_switch);
    builder->get_widget("enable_autostart", enable_autostart);
    builder->get_widget("enable_all_apps", enable_all_apps);
    builder->get_widget("use_default_sink", use_default_sink);
    builder->get_widget("use_default_source", use_default_source);
    builder->get_widget("input_device", input_device);
    builder->get_widget("output_device", output_device);
    builder->get_widget("show_spectrum", show_spectrum);

    get_object("buffer_in", buffer_in);
    get_object("buffer_out", buffer_out);
    get_object("latency_in", latency_in);
    get_object("latency_out", latency_out);
    get_object("spectrum_n_points", spectrum_n_points);

    auto flag = Gio::SettingsBindFlags::SETTINGS_BIND_DEFAULT;
    auto flag_invert_boolean =
        Gio::SettingsBindFlags::SETTINGS_BIND_INVERT_BOOLEAN;

    settings->bind("use-dark-theme", theme_switch, "active", flag);

    settings->bind("use-dark-theme", Gtk::Settings::get_default().get(),
                   "gtk_application_prefer_dark_theme", flag);

    settings->bind("enable-all-apps", enable_all_apps, "active", flag);

    settings->bind("use-default-sink", use_default_sink, "active", flag);

    settings->bind("use-default-sink", input_device, "sensitive",
                   flag | flag_invert_boolean);

    settings->bind("use-default-source", use_default_source, "active", flag);

    settings->bind("use-default-source", output_device, "sensitive",
                   flag | flag_invert_boolean);

    settings->bind("buffer-out", buffer_out, "value", flag);
    settings->bind("latency-out", latency_out, "value", flag);

    settings->bind("buffer-in", buffer_in, "value", flag);
    settings->bind("latency-in", latency_in, "value", flag);

    settings->bind("show-spectrum", show_spectrum, "active", flag);
    settings->bind("spectrum-n-points", spectrum_n_points, "value", flag);

    init_autostart_switch();

    enable_autostart->signal_state_set().connect(
        sigc::mem_fun(*this, &ApplicationWindow::on_enable_autostart), false);

    app->add_window(*window);

    window->show();
}

void ApplicationWindow::apply_css_style(std::string css_file_name) {
    auto provider = Gtk::CssProvider::create();

    provider->load_from_resource("/com/github/wwmm/pulseeffects/" +
                                 css_file_name);

    auto screen = Gdk::Screen::get_default();
    auto priority = GTK_STYLE_PROVIDER_PRIORITY_APPLICATION;

    Gtk::StyleContext::add_provider_for_screen(screen, provider, priority);
}

void ApplicationWindow::init_autostart_switch() {
    auto path =
        Glib::get_user_config_dir() + "/autostart/pulseeffects-service.desktop";

    try {
        auto file = Gio::File::create_for_path(path);

        bool exists = file->query_exists();

        if (exists) {
            enable_autostart->set_active(true);
        } else {
            enable_autostart->set_active(false);
        }
    } catch (const Glib::Exception& ex) {
        util::error(log_tag + ex.what());
    }
}

bool ApplicationWindow::on_enable_autostart(bool state) {
    auto path =
        Glib::get_user_config_dir() + "/autostart/pulseeffects-service.desktop";

    auto file = Gio::File::create_for_path(path);

    if (state) {
        try {
            Glib::RefPtr<Gio::FileOutputStream> stream;

            if (file->query_exists())
                stream = file->replace();
            else
                stream = file->create_file();

            stream->write("[Desktop Entry]\n");
            stream->write("Name=PulseEffects\n");
            stream->write("Comment=PulseEffects Service\n");
            stream->write("Exec=pulseeffects --gapplication-service\n");
            stream->write("Icon=pulseeffects\n");
            stream->write("StartupNotify=false\n");
            stream->write("Terminal=false\n");
            stream->write("Type=Application\n");
            stream->close();
            stream.reset();

            util::debug("application_window.cpp: autostart file created");
        } catch (const Glib::Exception& ex) {
            util::error("application_window.cpp: " + ex.what());
        }
    } else {
        try {
            file->remove();

            util::debug("application_window.cpp: autostart file removed");
        } catch (const Glib::Exception& ex) {
            util::error("application_window.cpp: " + ex.what());
        }
    }

    return false;
}
