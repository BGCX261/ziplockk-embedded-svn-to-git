#include <cairo.h>

int main()
{
	cairo_surface_t *surface;
	cairo_t *cr;

	surface = cairo_image_surface_create (
			CAIRO_FORMAT_ARGB32, 100, 20);
	cr = cairo_create (surface);
	cairo_save (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_rgba (cr, 1, 1, 1, 0);
	cairo_paint (cr);
	cairo_restore (cr);
	cairo_move_to (cr, 20, 20);
	cairo_show_text (cr, "Works here");
	cairo_destroy (cr);

	cairo_surface_write_to_png (surface, "out.png");
	cairo_surface_destroy (surface);

	return 0;
}

