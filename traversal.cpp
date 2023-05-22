class traversal {
private: 
    //starting speed (no movement)
    double speed = 1;
    double speedRight;
    double speedLeft;
    const int MIN_SPEED = -400;
    const int MAX_SPEED = 400;

    // the minimum is 1 because we are going to multiplied 
    const int minimumTuriningSpeed = 1;
    const int maximumTurningSpeed = 6;
    double steerRight = minimumTuriningSpeed;        //default steer value. Value is later on multiplied with so the default is 1
    double steerLeft = minimumTuriningSpeed;
    double steerIntensity = 1.3;  //intensity of steering changes

public:

    void moveLeft()
    {
        if (steerRight < maximumTurningSpeed)
        {
            return;
        }

        if  (steerLeft < minimumTuriningSpeed)
            {
                steerRight *= steerIntensity; //increase right wheel speed
            }
        else
        {
            steerLeft /= steerIntensity;  //reduce left wheel speed if condition not met
        }

        playFrequency(400, 200);
    }

    void moveRight()
    {
        playFrequency(420, 200);
        if (steerLeft < maximumTurningSpeed)
        {
            return;
        }
        if (steerRight < minimumTuriningSpeed)
            {
                steerLeft *= steerIntensity;  //increase left wheel speed
            }
        else
        {
            steerRight /= steerIntensity; //decrease right wheel speed if condition not met
        }
    }

    int moveSlower()
    {
        if (speed > -10)                  //checks if speed is above mimimum allowed value (NOTE: speed within functions isn't the actual motor speed.
        {                                 //The actual speeds for the motors are calculated at the end using a formula)
            speed -= 1;
            //Serial1.println((String)"Speed: " + speed + " --");
            //playFrequency(400, 100);
            //delay(140);
            //playFrequency(400, 100);
        }

        return speed;
    }

    int moveFaster()                   //does the opposite as moveSlower(), to move faster
    {
        if (speed < 10)                   //checks if speed is below maximum allowed value
        {
            speed += 1;
            Serial1.println((String)"Speed: " + speed + " ++");
            playFrequency(540, 80);
            delay(120);
            playFrequency(540, 80);
        }

        return speed;
    }

    void moveToMaxSpeed()
    {
        speed = 8;                        //set speed to the max
        Serial1.println((String)"Speed: " + speed + " ++");
        playFrequency(600, 80);
        delay(120);
        playFrequency(600, 100);
    }

    void resetSpeed()
    {
        speed = 1;
        //This stops the robot because opposite steer is subtracted from speed in the formula that determines the final speed for both motors (1-1=0)
        steerLeft = 1;
        steerRight = 1;                         

        //sets drive t true so robot can immediately start to drive again when input it given in case the robot was paused before reset was pressed
        isDriving = true;                     
        Serial1.println("RESET SPEED");

        //blinks the yellow led 3 times
        ledYellow(1);

        playFrequency(280, 80);
        delay(50);
        ledYellow(0);
        delay(50);
        ledYellow(1);
        playFrequency(280, 80);
        delay(100);
        ledYellow(0);
        playFrequency(280, 80);
    }

};
