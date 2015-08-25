#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <cairo-xlib.h>
#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SIZEX 720
#define SIZEY 580

void slider(cairo_t *cr,
	    double x,
	    double y,
            double thickness,
	    double width,
	    double height,
	    double r,
	    double g,
	    double b,
	    double percentage)
{
	cairo_move_to(cr,(x-(width/2))-5,(y-(height/2))-5);
	cairo_set_line_width(cr,thickness);
	cairo_set_source_rgba(cr,0.0,0.0,0.0,0.25);
	cairo_rectangle(cr,(x-(width/2))-5,(y-(height/2))-5,width+10,height+10);
	cairo_fill(cr);

	cairo_move_to(cr,(x-(width/2)),(y-(height/2)));
	cairo_set_line_width(cr,thickness);
	cairo_set_source_rgb(cr,1.0,1.0,1.0);
	cairo_rectangle(cr,(x-(width/2)),(y-(height/2)),width,height);
	cairo_stroke(cr);
	cairo_move_to(cr,(x-(width/2))+thickness,(y-(height/2))+thickness);
	cairo_set_source_rgb(cr,r,g,b);
	cairo_rectangle(cr,(x-(width/2))+thickness,(y-(height/2))+thickness,(width-(2*thickness))*(percentage/100),height-(2*thickness));
	cairo_fill(cr);
}

void filledCircle(cairo_t *cr,
		  double x,
		  double y,
		  double thickness,
		  double radius,
		  double r,
		  double g,
		  double b,
		  double alpha)
{
	cairo_move_to(cr,x+radius,y);
	cairo_set_line_width(cr,thickness);
	cairo_set_source_rgba(cr,r,g,b,alpha);
	cairo_arc(cr,x,y,radius,0.0,2*M_PI);
	cairo_fill(cr);
}

void unfilledCircle(cairo_t *cr,
		    double x,
		    double y,
		    double thickness,
		    double radius,
		    double r,
		    double g,
		    double b)
{
	cairo_move_to(cr,x+radius,y);
	cairo_set_line_width(cr,thickness);
	cairo_set_source_rgb(cr,r,g,b);
	cairo_arc(cr,x,y,radius,0.0,2*M_PI);
	cairo_stroke(cr);
}

void renderGdot(cairo_t *cr,
		double x,
		double y,
		double gX,
		double gY,
		double gZ)
{
	double oneGradius=20.0; // radius that equates to 1G
	double dotRadius=10*(gZ/1.0); // scale center dot base on Z axis G
	double dotX=x+(gX*oneGradius);
	double dotY=y+(gY*oneGradius);

	filledCircle(cr,x,y,2.0,(2*oneGradius)+5,0.0,0.0,0.0,0.25);
	unfilledCircle(cr,x,y,2.0,2*oneGradius,1.0,1.0,1.0);
	unfilledCircle(cr,x,y,2.0,oneGradius,1.0,1.0,1.0);
	filledCircle(cr,dotX,dotY,2.0,dotRadius,1.0,0.0,0.0,1.0);
}

int generateFrame(char *frameName,
		  unsigned int frameNumber,
		  double gX,
		  double gY,
		  double gZ,
		  unsigned int speed,
		  double throttlePercentage,
		  double brakePercentage)
{
	cairo_surface_t *surface;
	cairo_t *cr;
	char frameText[80];


	surface = cairo_image_surface_create (
			CAIRO_FORMAT_ARGB32, SIZEX, SIZEY);
	cr = cairo_create (surface);
	cairo_save (cr);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	// set alpha to produce transparency
	cairo_set_source_rgba (cr, 1, 1, 1, 0);
//	cairo_set_source_rgba (cr, 0, 0, 0.2, 1);
	cairo_paint (cr);
	cairo_restore (cr);
	cairo_move_to (cr, 17.5, 20);
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_set_font_size(cr,20);
	sprintf(frameText,"%05u",frameNumber);
	cairo_show_text (cr, frameText);

//	G bubble
	renderGdot(cr,50,70,gX,gY,gZ);

//	throttle
	slider(cr,50,130,2,80,20,0.0,1.0,0.0,throttlePercentage);

//	brake
	slider(cr,50,160,2,80,20,1.0,0.0,0.0,brakePercentage);

	// save and clean up
	cairo_destroy (cr);
	cairo_surface_write_to_png (surface, frameName);
	cairo_surface_destroy (surface);

	return 0;
}

int main(int argc, char *argv[])
{
	char fileName[80];
	unsigned int loop=0;
	unsigned int offset=0;
	
	for(loop=1;loop<3000;loop++)
	{
		double gX=2.0*sin((2*M_PI*((double)(loop%25)/25.0)));
		double gY=2.0*sin((2*M_PI*((double)((loop+offset)%25)/25.0)));
		double gZ=1.0-0.5*sin((2*M_PI*((double)((loop+10+offset)%25)/25.0)));
		unsigned int speed=0;

		sprintf(fileName,"frames/frame%08u.png",loop);

		if(generateFrame(fileName,
				 loop,
				 gX,
				 gY,
				 gZ,
				 speed,
				 ((double)(loop%40)/40)*100,
				 ((double)(loop%30)/30)*100))
		{
			return 1;
		}

		if(!(loop%12))
		{
			offset++;
		}
	}

	return 0;
}

