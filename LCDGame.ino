
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

//variables for program
int adc_key_in;
int position = 5;
int adc_key_val[5] = {50, 200, 400, 600, 800 }; 
int NUM_KEYS = 5;
int repeats = 0;
int updateTime = 0;
long resetTime = 0;
int score = 0;

//array for which positions have spikes
int spikes[2][14] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

//********************************codes for characters --> spikes, block******************
//first position for block
byte first[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110
};

//second position for block
byte second[8] = {
  0b00000,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b00000,
  0b00000
};

//third position for block
byte third[8] = {
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

//fourth position for block
byte fourth[8] = {
  0b01110,
  0b01110,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

//top of third position
byte fifth[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01110
};

//top of fourth position
byte sixth[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b01110,
  0b01110
};

//lower spike
byte spike[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00100,
  0b01100,
  0b01100,
  0b01110,
  0b11111
};

//upper spike
byte uspike[8] = {
  0b11111,
  0b01110,
  0b01110,
  0b00100,
  0b00100,
  0b00000,
  0b00000,
  0b00000
};

//*********************************METHODS********************************************
boolean get_key(unsigned int input) //check if a button is pressed
{
    int k;

   //checks if each button is pressed
    for (k = 0; k < NUM_KEYS; k++) {
      if (input < adc_key_val[k])
        return true;        //button is pressed if condition is met
    }

    if (k >= NUM_KEYS)             // No valid key pressed
      k = -1;

    //returns whether or not valid key was pressed (and if not done earlier)
    boolean pressed = (k >= 0);
    return pressed;
}

//method to make spikes move, generate new + random spikes
void updateSpikes()
{
  //method checks each row and column where spikes can exist (2 rows, 14 columns)
  for (int row = 0; row < 2; row++)
  {
    for (int col = 0; col < 14; col++)
    {
      //checks if value in array is not 0 --> contains spike
      if (spikes[row][col] != 0)
      {
        //moves spike one to the left (column-1) and makes current block empty
        spikes[row][col] = 0;
        if (col > 0)
          spikes[row][col-1] = 1;
      }
    }
  }

  //************NEW SPIKES****************
  //if second last column has spike at the top AND not many repeats
  if (spikes[0][12] == 1 && repeats < 3)
  {
    //generate a random number from 0-6
    int x = random(0,6);
    if (x % 2 == 0)   //if even, adds a new spike to the top row
      spikes[0][13] = 1;

    repeats++;      //adds one to repeated numbers
  }

  //if second last column has spike at the bottom AND not many repeats
  else if (spikes[1][12] == 1 && repeats <3)
  {
    int x = random(0,6);    //generate random number from 0-6
    if (x % 2 == 0)
      spikes[1][13] = 1;    //if even, adds a new spike to the bottom row

      repeats++;
  }

  //if spikes are not present if either second or third last columns
  else if (spikes[0][12] != 1 && spikes[1][12] != 1 && spikes[0][11] != 1 && spikes[1][11] != 1)
  {
    int x = random(0,6);    //generates random number from 1 to 6
    if (x%3 == 0)
      spikes[0][13] = 1;    //if divisible by 3, spike on top row
    else if (x%3 == 1)
      spikes[1][13] = 1;    //if remainder is 1, spike on bottom row

    repeats = 0;    //resets repeat count
  }
}

//setup method
void setup()
{  
  // create new custom characters for block spikes
  lcd.createChar(0, first);
  lcd.createChar(1, second);
  lcd.createChar(2, third);
  lcd.createChar(3, fourth);
  lcd.createChar(4, fifth);
  lcd.createChar(5, sixth);
  lcd.createChar(6, spike);
  lcd.createChar(7, uspike);
  //random seed for random numbers
  randomSeed(42);
  
  // set up number of columns and rows
  lcd.begin(16, 2);

}

void loop() 
{
  //clear screen, get start time for the loop
  lcd.clear();
  long startTime = millis();

  //outputs each digit in score in last two columns 
  lcd.setCursor(15,1);
  lcd.write(score % 10+48);
  lcd.setCursor(14,1);
  lcd.write((score %100)/10+48);
  lcd.setCursor(15,0);
  lcd.write((score%1000)/100+48);
  lcd.setCursor(14,0);
  lcd.write((score/1000)+48);
    
  adc_key_in = analogRead(0);      // read the value from the sensor 
  boolean pressed = get_key(adc_key_in);       // convert into key press

  //if position is less or equal to three (part of block is in lower row)
  if (position <=3)
  {
    //draw block
    lcd.setCursor(1,1);
    lcd.write((uint8_t)position);

    //draw part of block in upper row if block is in higher position --> jump
    if (position == 2 || position == 3)
    {
      lcd.setCursor(1,0);
      lcd.write((uint8_t)position+2);
    }
  }

  //if position > 3 (block all in upper row)
  else if (position > 3)
  {
    //draw in block in upper row
    lcd.setCursor(1,0);
    lcd.write((uint8_t)position-4);
  }

  //if button is pressed and position less than five
  if (pressed && position <=4)
  {
    //add to position --> block goes up
    position++;
  }

  //if button not pressed and position more than 0
  if (!pressed && position > 0)
  {
    //remove from position --> block drops
    position--;
  }

  //run through each element in array of spikes
  for (int row = 0; row < 2; row++)
  {
    for (int col = 0; col < 14; col++)
    {
      //if any element is not a 0, draw spike in given location
      if (spikes[row][col] != 0)
      {
        lcd.setCursor(col,row);
        if (row == 0)
          lcd.write((uint8_t)7);
        else if (row == 1)
          lcd.write((uint8_t)6);
      }
    }
  }

  //speed change --> update spikes faster and faster as game progresses
  int time = millis();

  //if score < 10, updates spikes every .3 seconds
  if (score < 10)
  {
    if (time - updateTime > 300)
    {
      updateSpikes();
      updateTime = time;  
    }
  }

  //if score < 20, updates spikes every .275 seconds
  else if (score < 20)
  {
    if (time - updateTime > 275)
    {
      updateSpikes();
      updateTime = time;  
    }
  }

  //if score < 30, updates spikes every .25 seconds
  else if (score < 30)
  {
    if (time - updateTime > 250)
    {
      updateSpikes();
      updateTime = time;  
    }
  }

  //if score < 40, updates spikes every .225 seconds
  else if (score < 40)
  {
    if (time - updateTime > 225)
    {
      updateSpikes();
      updateTime = time;  
    }
  }

  //if score < 50, updates spikes every .2 seconds
  else if (score < 50)
  {
    if (time - updateTime > 200)
    {
      updateSpikes();
      updateTime = time;  
    }
  }

  //if score < 75, updates spikes every .16 seconds
  else if (score < 75)
  {
    if (time - updateTime > 160)
      {
        updateSpikes();
        updateTime = time;  
      }
  }

  //if score < 100, updates spikes every .125 seconds
  else if (score < 100)
  {
    if (time - updateTime > 125)
      {
        updateSpikes();
        updateTime = time;  
      }
  }

  //if score is above all parameters, updates spikes every .09 seconds
  else 
  {
    if (time - updateTime > 90)
      {
        updateSpikes();
        updateTime = time;  
      }
  }

  //checks if spike in column 2 every overlaps with the block's position
  if ((spikes[0][1] == 1 && position > 2) || (spikes[1][1] == 1 && position < 2))
  {
    //if yes, show game over on screen
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.write("Game Over!!");
    lcd.setCursor(1,1);
    lcd.write("Score:");

    //output digits for the score
    lcd.write((score/1000)+48);
    lcd.write((score%1000)/100+48);
    lcd.write((score %100)/10+48);
    lcd.write(score % 10+48); 

    //reset position to 6
    position = 6;

    //reset all the spikes to 0
    for (int row = 0; row < 2; row++)
    {
      for (int col = 0; col < 14; col++)
      {
        spikes[row][col] = 0;
      }
    }

    //three second pause, then game starts again
    delay(3000);

    //new value for reset time
    resetTime = millis();
  }

  //score equals total run time of program - reset time / 500 milliseconds
  score = (startTime - resetTime) / 500;

  delay(50);
  
}
