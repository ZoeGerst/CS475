


#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

unsigned int seed = 0;




int	NowYear;		// 2023 - 2028
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// rye grass height in inches
int	NowNumRabbits;		// number of rabbits in the current population
int NowNumFoxes;


const float RYEGRASS_GROWS_PER_MONTH =		20.0;
const float ONE_RABBITS_EATS_PER_MONTH =	 1.0;

const float AVG_PRECIP_PER_MONTH =	       12.0;	// average
const float AMP_PRECIP_PER_MONTH =		4.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				60.0;
const float MIDPRECIP =				14.0;

omp_lock_t	Lock;
volatile int	NumInThreadTeam;
volatile int	NumAtBarrier;
volatile int	NumGone;





float
Ranf( unsigned int *speedp, float low, float high )
{
        float r = (float) rand_r( speedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

float x = Ranf( &seed, -1.f, 1.f );

float
Sqr( float x )
{
        return x*x;
}

// specify how many threads will be in the barrier:
// //	(also init's the Lock)

void
InitBarrier( int n )
{
    NumInThreadTeam = n;
	NumAtBarrier = 0;
	omp_init_lock( &Lock );
}


// have the calling thread wait here until all the other threads catch up:

void
WaitBarrier( )
{
	omp_set_lock( &Lock );
    {
    	NumAtBarrier++;
        if( NumAtBarrier == NumInThreadTeam )
        {
        	NumGone = 0;
            NumAtBarrier = 0;
           	// let all other threads get back to what they were doing
            // before this one unlocks, knowing that they might immediately
            // call WaitBarrier( ) again:
            while( NumGone != NumInThreadTeam-1 );
            	omp_unset_lock( &Lock );
                return;
            }
	}
    omp_unset_lock( &Lock );

    while( NumAtBarrier != 0 );	// this waits for the nth thread to arrive

    #pragma omp atomic
    NumGone++;			// this flags how many threads have returned
}

void Rabbits(){

	while(NowYear < 2029){

		int nextNumRabbits = NowNumRabbits;
		int carryingCapacity = (int)( NowHeight );

		int predator = (int)(NowNumFoxes);

		if( nextNumRabbits < carryingCapacity ){
        	nextNumRabbits++;

		}
		else{
        	if( nextNumRabbits > carryingCapacity ){
            	nextNumRabbits--;

			}
		}

		if(nextNumRabbits > predator){

			nextNumRabbits--;

		}

		if( nextNumRabbits < 0 ){
        	nextNumRabbits = 0;

		}

		WaitBarrier( );

		NowNumRabbits = nextNumRabbits;


		WaitBarrier( );

		WaitBarrier( );

	}

}

void RyeGrass(){

	while(NowYear < 2029){

		float nextHeight = NowHeight;

		float tempFactor = exp(   -Sqr(  ( NowTemp - MIDTEMP ) / 10.  )   );

		float precipFactor = exp(   -Sqr(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

 		nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
 		nextHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;

		if( nextHeight < 0 ){
            nextHeight = 0;

        }

		WaitBarrier( );

		NowHeight = nextHeight;


		WaitBarrier( );

		WaitBarrier( );

	}

}

void MyAgent(){

	while(NowYear < 2029){

		int NextNumFoxes = NowNumFoxes;

		if(NowNumFoxes < NowNumRabbits){

			NextNumFoxes = NextNumFoxes + 2;

		}
		else {
				if(NowNumFoxes > NowNumRabbits){

					NextNumFoxes--;
				}

		}
		if(NextNumFoxes < 0){

			NextNumFoxes = 0;

		}
		WaitBarrier( );

		NowNumFoxes = NextNumFoxes;



		WaitBarrier( );

		WaitBarrier( );

	}

}

void Watcher(){

	while(NowYear < 2029){

		WaitBarrier( );

		WaitBarrier( );
	
	
		printf("%d, %d, %f, %f, %f, %d, %d\n", NowYear, NowMonth, (5./9.) * (NowTemp - 32.), NowPrecip * 2.54, NowHeight * 2.54, NowNumRabbits, NowNumFoxes);

		if(NowMonth >= 11){

			NowMonth = 0;
			NowYear++;

		}
		else{

			NowMonth++;

		}


		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
		NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
		if( NowPrecip < 0. ){

			NowPrecip = 0.;
		}

		WaitBarrier( );


	}
}






int main(){

	// starting date and time:
	
	NowMonth =    0;
	NowYear  = 2023;

	// starting state (feel free to change this if you want):
	NowNumRabbits = 1;
	NowHeight =  5.;
	NowNumFoxes = 0;


	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip < 0. ){
		NowPrecip = 0.;
	}
	
	
	
	omp_set_num_threads( 3 );	// or 4
	InitBarrier( 3 );		// or 4

	#pragma omp parallel sections
	{	
		#pragma omp section
		{
			Rabbits( );
		}

		#pragma omp section
		{
			RyeGrass( );
		}

		#pragma omp section
		{
			Watcher( );
		}

		#pragma omp section
		{
			MyAgent( );	// your own
		}
	}       // implied barrier -- all functions must return in order
	// to allow any of them to get past here

}
