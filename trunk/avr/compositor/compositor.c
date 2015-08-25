#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>

void ThrowWandException(MagickWand *wand)
{
	char *description;

	ExceptionType severity;

	description=MagickGetException(wand,&severity);
	fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description);
	description=(char *) MagickRelinquishMemory(description);
	//exit(-1);
}

void compositeImages(char *imageName, char *frameName, char *outputName)
{
	MagickBooleanType status;
	MagickWand *image;
	MagickWand *frame;

	// Read an image.
	image=NewMagickWand();
	status=MagickReadImage(image,imageName);
	if (status==MagickFalse)
	{
		ThrowWandException(image);
		image=DestroyMagickWand(image);
		return;
	}

	frame=NewMagickWand();
	status=MagickReadImage(frame,frameName);
	if (status==MagickFalse)
	{
		ThrowWandException(frame);
		frame=DestroyMagickWand(frame);
		return;
	}

	// composite overlay.
	MagickCompositeImage(image,frame,OverCompositeOp,0,0);

	// Write the image then destroy it.
	status=MagickWriteImages(image,outputName,MagickTrue);

	if(status==MagickFalse)
	{
		ThrowWandException(image);
	}
	
	image=DestroyMagickWand(image);
	frame=DestroyMagickWand(frame);
}

int main(int argc,char **argv)
{
	char imageName[80];
	char frameName[80];
	char outputName[80];

	MagickWandGenesis();

	for(int loop=1;loop<3000;loop++)
	{
		sprintf(imageName,"/home/fergusd/Videos/image%08d.png",loop);
		sprintf(frameName,"../cairo/frames/frame%08d.png",loop);
		sprintf(outputName,"images/output%08d.png",loop);
		printf("process %s:%s:%s\n",frameName,imageName,outputName);
		compositeImages(imageName,frameName,outputName);
	}

	MagickWandTerminus();

	return(0);
}
