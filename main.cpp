#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define f32 float

class Demo : public olc::PixelGameEngine 
{
private: 
    f32 **grid;
    int WIDTH, HEIGHT;
    int N;

private:

    f32 frand() {
        return ((f32)rand() / (RAND_MAX)) * 2 - 1;
    }

    f32 round(f32 val) {
        return floor(val + 0.5);
    }

    f32 mapRange(f32 input, f32 inputStart, f32 inputEnd, f32 outputStart, f32 outputEnd) {
        double slope = 1.0 * (outputEnd - outputStart) / (inputEnd- inputStart);
        return outputStart + round(slope * (input - inputStart));
    }

    f32** allocateGrid(int N) {
        //int gridSize = (int)(pow(2, N) + 1);
        int gridSize = N;

        f32 **grid = new f32*[gridSize];
        
        for (int y=0; y<gridSize; y++) {
            grid[y] = new f32[gridSize];
            for (int x=0; x<gridSize; x++) {
                grid[y][x] = 0;
            }
        }
        return grid;
    }

    f32 sample(int x, int y)
    {
        int iX = (x + WIDTH) % WIDTH;
        int iY = (y + HEIGHT) % HEIGHT;
        return grid[iY][iX];
    }
     
    void setSample(int x, int y, f32 value)
    {
        int iX = (x + WIDTH) % WIDTH;
        int iY = (y + HEIGHT) % HEIGHT;
        grid[iY][iX] = value;
    } 

    void sampleSquare(int x, int y, int size, f32 randval)
    {
        // a     b 
        //
        //    x
        //
        // c     d
     
        int hs = size / 2;
        f32 a = sample(x - hs, y - hs);
        f32 b = sample(x + hs, y - hs);
        f32 c = sample(x - hs, y + hs);
        f32 d = sample(x + hs, y + hs);
     
        setSample(x, y, ((a + b + c + d) / 4.0) + randval);
    }

    void sampleDiamond(int x, int y, int size, f32 randval)
    {
        //   c
        //
        //a  x  b
        //
        //   d
     
        int hs = size / 2;
        f32 a = sample(x - hs, y);
        f32 b = sample(x + hs, y);
        f32 c = sample(x, y - hs);
        f32 d = sample(x, y + hs);
     
        setSample(x, y, ((a + b + c + d) / 4.0) + randval);
    }   

    void DiamondSquare(int stepsize, double scale)
    {
        int halfstep = stepsize / 2;
        
        // diamond step, creates new squares
        for (int y = halfstep; y < HEIGHT + halfstep; y += stepsize) {
            for (int x = halfstep; x < WIDTH + halfstep; x += stepsize) {
                sampleSquare(x, y, stepsize, frand() * scale);
            }
        }

        // square step, creates new diamonds 
        for (int y = 0; y < HEIGHT; y += stepsize) {
            for (int x = 0; x < WIDTH; x += stepsize) {
                sampleDiamond(x + halfstep, y, stepsize, frand() * scale);
                sampleDiamond(x, y + halfstep, stepsize, frand() * scale);
            }
        }
    }

public:
    Demo() {
        sAppName = "Demo";
    }

    // Init
    bool OnUserCreate() override {
        SetPixelMode(olc::Pixel::ALPHA);
        HEIGHT = ScreenHeight();
        WIDTH = ScreenWidth(); 
        grid = allocateGrid(HEIGHT);

        int featuresize = HEIGHT / 2;

        for(int y = 0; y < HEIGHT; y += featuresize)  
            for(int x = 0; x < WIDTH; x += featuresize) 
                setSample(x, y, frand()*2.0);

        int samplesize = featuresize;
        f32 scale = 1.0;
         
        while (samplesize > 1) {
            DiamondSquare(samplesize, scale);
            samplesize /= 2;
            scale /= 2.0;
        }

        Clear(olc::BLACK);
        for(int y = 0; y < HEIGHT; y++) { 
            for(int x = 0; x < WIDTH; x++) { 
                f32 current = sample(x, y);
                int colorVal = (int)mapRange(current, -2.0, 2.0, 0.0, 255.0);
                Draw(x, y, olc::Pixel(colorVal, colorVal, colorVal, 255));
            }
        }

        return true;
    }

    // Update
    bool OnUserUpdate(float fElapsedTime) override {
        //Clear(olc::BLACK);
        //for (int y=0; y<HEIGHT; y++) {
        //    for (int x=0; y<WIDTH; x++) {
        //        int gridValue = (int)sample(x, y);
        //        Draw(x, y, olc::Pixel(0, 0, gridValue % 255));
        //    }
        //}
        return true;
    }

    bool OnUserDestroy() override {
       for(int y = 0; y < HEIGHT; y++)
           delete [] grid[y];
        delete [] grid;

        return true;
    }
};


int main(int argc, char const *argv[]) {
    srand(time(NULL));
    
    int gridSize = 32;

	Demo demo;
	if (demo.Construct(gridSize*4, gridSize*4, 4, 4))
		demo.Start();
	return 0;
}
