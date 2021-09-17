
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// In order to load the resources like cute_image.png, you have to set up
// your target scheme:
//
// - Select "Edit Scheme…" in the "Product" menu;
// - Check the box "use custom working directory";
// - Fill the text field with the folder path containing your resources;
//        (e.g. your project folder)
// - Click OK.
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>

struct ScalesObj{
    float topRowScale;
    float btmRowScale;
    float leftColScale;
    float rightColScale;
    
    float topRowPrevScale;
    float btmRowPrevScale;
    float leftColPrevScale;
    float rightColPrevScale;
    
    
    float topRowCurrScale;
    float btmRowCurrScale;
    float leftColCurrScale;
    float rightColCurrScale;
    
};


sf::Image srcBitmapData;
sf::Image targetBitmapData;
sf::Texture targetBDTexture;
sf::Sprite sprite;
int stageWidth = 687;
int stageHeight = 1024;
float srcImageWidth;
float srcImageHeight;
bool tweenComplete = true;
float duration = 50;
float animTime = 0;

float easeOutQuad(float _time, float _start, float _end, float _duration) {
    
    _time /= _duration;
    float res = -_end * _time * (_time - 2) + _start;
    
    return res;
    
};

float linearEase(float _time, float _start, float _end, float _duration) {
    
    float len = std::abs(_end - _start);
    //printf("len %f \n", len);
    float per = _time / _duration;
    //printf("per %f \n", per);
    float curr = len * per;
    //printf("curr %f \n", curr);
    
    if(_start < _end)
    {
        return curr + _start;
    }
    else
    {
        return _start - curr;
    }
    
    return curr;//_end * _time / _duration + _start;
}


void drawImageToCanvas(ScalesObj * scalesObj) {
    //get length of each col
    float leftColLen = srcImageHeight * scalesObj->leftColCurrScale;
    float rightColLen = srcImageHeight * scalesObj->rightColCurrScale;
    float colLen = rightColLen - leftColLen;//get the different in pixels between the start and end column
    float numIterations = srcImageWidth;//we need to get from left to right over numIterations
    std::vector<float> pixelColsArr;
    float per;
    //the first col length will be the left, the last col length will be the right, now we need the length of all cols in betwee from a to b.
    //we do this by adding a percentage to the left col on each step and saving the length in an array
    for (float i  = 0; i < numIterations; i++) {
        per = i / numIterations;
        float numPixelsPerCol  = (colLen * per) + leftColLen;
        pixelColsArr.push_back(numPixelsPerCol);
    }
    
    
    //now we do the same for the rows
    //get the length of the bottom row and the length of the top row.
    //the dirrence between them is the percenteage the row needs to grow on each step
    //get length of each row
    float topRowLen = srcImageWidth * scalesObj->topRowCurrScale;
    float btmRowLen = srcImageWidth * scalesObj->btmRowCurrScale;
    
    float rowLen = btmRowLen - topRowLen;
    numIterations = srcImageHeight;
    std::vector<float> pixelRowsArr;
    
    for (float i = 0; i < numIterations; i++) {
        per = i / numIterations;
        float numPixelsPerRow  = (rowLen * per) + topRowLen;
        pixelRowsArr.push_back(numPixelsPerRow) ;
    }
    
    
    
    float row = 0;
    float col;
    sf::Color pixel;
    float trueRow = 0;
    for (row = srcImageHeight - 1; row >= 0; row--)
    {
        //get the length of the current row to draw
        float _rowLen = pixelRowsArr[row];
        //the start col is the total width - the length of the row to draw. we divide by 2 to center the image
        float startCol = (srcImageWidth - _rowLen) / 2;
        //we draw from the start col to the start col + the length of our row
        float rowEnd = _rowLen + startCol;
        float colJumpPer = srcImageWidth / _rowLen;
        
        for (col = srcImageWidth-1; col >= 0; col--)
        {
            float _colLen = pixelColsArr[col];
            float startRow = (srcImageHeight - _colLen) / 2;
            float colEnd  = startRow + _colLen;
            float rowJumpPer = srcImageHeight / _colLen;
            
            //if we are in the bounds of the len, draw the image
            if ((col >= startCol && col <= rowEnd) && (row >= startRow && row <= colEnd))
            {
                //get the pixel in the original image, starting from 0,0 - and paint it to the point in the new canves, which has to be in the bounds.
                //casting to int means we get an actual pixel
                pixel = srcBitmapData.getPixel(int((col - startCol) * colJumpPer), int((row - startRow) * rowJumpPer));
                targetBitmapData.setPixel(col, row, pixel);
            }
            else
            {
                targetBitmapData.setPixel(col, row, sf::Color::Black);
            }
        }
    }
}


int main(int argc, char const** argv)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(stageWidth, stageHeight), "SFML window");
    window.setFramerateLimit(60);

    // Set the Icon
    
    srcBitmapData.loadFromFile("mona_lisa.jpg");
    
    ScalesObj scalesObj;
    scalesObj.btmRowScale = 1;
    scalesObj.leftColScale = 1;
    scalesObj.rightColScale = 1;
    scalesObj.topRowScale = 1;
    
    scalesObj.btmRowCurrScale = 1;
    scalesObj.leftColCurrScale = 1;
    scalesObj.rightColCurrScale = 1;
    scalesObj.topRowCurrScale = 1;
    
    
    srcImageWidth = srcBitmapData.getSize().x;
    srcImageHeight =  srcBitmapData.getSize().y;
    
    targetBitmapData.create(srcImageWidth, srcImageHeight);
    
    
    //drawImageToCanvas(&scalesObj);

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // Clear screen
        window.clear();
        
        
        
        
        if(tweenComplete)
        {
            scalesObj.btmRowPrevScale   = scalesObj.btmRowScale;
            scalesObj.leftColPrevScale  = scalesObj.leftColScale;
            scalesObj.rightColPrevScale = scalesObj.rightColScale;
            scalesObj.topRowPrevScale   = scalesObj.topRowScale;
            
            int r = (rand() % 100) + 1;
            scalesObj.btmRowScale = (float)r / 100;
            
            r = (rand() % 100) + 1;
            scalesObj.leftColScale  = (float)r / 100;
            
            r = (rand() % 100) + 1;
            scalesObj.rightColScale  = (float)r / 100;
            
            r = (rand() % 100) + 1;
            scalesObj.topRowScale  = (float)r / 100;
            
            //printf("btmRowPrevScale %f, btmRowScale %f \n", scalesObj.btmRowPrevScale, scalesObj.btmRowScale);
            //printf("leftColScale %f \n", scalesObj.leftColScale);
            //printf("rightColScale %f \n", scalesObj.rightColScale);
            //printf("topRowScale %f \n", scalesObj.topRowScale);
            //printf("--------------\n");
            
            tweenComplete = false;
            animTime = 0;
            
        }
        else
        {
            scalesObj.btmRowCurrScale   = linearEase( animTime, scalesObj.btmRowPrevScale,   scalesObj.btmRowScale,   duration);
            scalesObj.leftColCurrScale  = linearEase( animTime, scalesObj.leftColPrevScale,  scalesObj.leftColScale,  duration);
            scalesObj.rightColCurrScale = linearEase( animTime, scalesObj.rightColPrevScale, scalesObj.rightColScale, duration);
            scalesObj.topRowCurrScale   = linearEase( animTime, scalesObj.topRowPrevScale,   scalesObj.topRowScale,   duration);
            
            //printf("btmRowPrevScale %f \n", scalesObj.btmRowPrevScale);
            //printf("btmRowCurrScale %f \n", scalesObj.btmRowCurrScale);
            //printf("btmRowScale %f \n", scalesObj.btmRowScale);
            
            drawImageToCanvas(&scalesObj);
            animTime++;
            
            
            if(animTime == duration)
            {
                //printf("YO \n");
                tweenComplete = true;
            }
        }
        
        targetBDTexture.loadFromImage(targetBitmapData);
        sprite.setTexture(targetBDTexture, true);

        // Draw the sprite
        window.draw(sprite);


        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
