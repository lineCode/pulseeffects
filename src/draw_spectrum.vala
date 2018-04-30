public class DrawSpectrum : Object {
    private ApplicationWindow app;
    private double mouse_freq;
    private double mouse_intensity;

    public DrawSpectrum (ApplicationWindow app) {
        this.app = app;

        app.spectrum.draw.connect(this.on_draw);
        app.spectrum.enter_notify_event.connect(this.on_enter_notify_event);
        app.spectrum.leave_notify_event.connect(this.on_leave_notify_event);
        app.spectrum.motion_notify_event.connect(this.on_motion_notify_event);
    }

    private bool on_draw(Cairo.Context ctx) {
        ctx.paint();

        return false;
    }

    private bool on_enter_notify_event(Gdk.EventCrossing event) {
        return false;
    }

    private bool on_leave_notify_event(Gdk.EventCrossing event) {
        return false;
    }

    private bool on_motion_notify_event(Gdk.EventMotion event) {
        var allocation = Gtk.Allocation();

        this.app.spectrum.get_allocation(out allocation);

        int width = allocation.width;
        int height = allocation.height;

        this.mouse_intensity = -event.y * 120 / height;

        return false;
    }
}