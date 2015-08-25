#include "tc.h"
#include "freq.h"

volatile uint16_t frontSpeed=0;
volatile uint16_t rearSpeed=0;
volatile uint16_t slip=0;
volatile uint8_t status=0;

void tcCheck()
{
	if((freqValue[0]==0xffff)||
	   (freqValue[1]==0xffff)||
	   (freqValue[2]==0xffff))
	{
		status=0;

		frontSpeed=0;
		rearSpeed=0;
		slip=0;
	}
	else
	{
		status=1;

		uint32_t tempA=freqValue[0];
		tempA+=freqValue[1];
		tempA=tempA/2;
		frontSpeed=(uint16_t)tempA;
		rearSpeed=freqValue[2];

		if(frontSpeed>rearSpeed)
		{
			//tempA=(frontSpeed-rearSpeed)*100;
			tempA=tempA-(uint32_t)(rearSpeed);
			tempA=tempA*100;
			uint32_t tempB=(uint32_t)rearSpeed;
			uint32_t slip32=(tempA/tempB);
			slip=(uint16_t)slip32;
		}
		else
		{
			slip=0xffff;
		}
	}	
}

void tcInit()
{
}

