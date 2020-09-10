

#define NOT_ASSIGNED -1

enum gcmode { MODE_DIRECTIONAL, MODE_ABSOLUTEPOS, MODE_RELATIVEPOS, MODE_NONE };

struct GameController
{
    gcmode mode = MODE_DIRECTIONAL;
    int id = 0;     // device number assigned by client 
    bool active = false;
        
    GameController( int id_, gcmode mode_)
    {
      id = id_;
      mode = mode_;        
    }

    virtual bool isLeft()     { return false; };
    virtual bool isRight()    { return false; };
    virtual bool isUp()       { return false; };
    virtual bool isDown()     { return false; };
    virtual bool isButtonA()  { return false; }; 
    virtual bool isButtonB()  { return false; };

    virtual void update()  { };

    virtual int getDX()       { return 0; }
    virtual int getDY()       { return 0; }
    
};

struct GameControllerKeys : public GameController
{
  GameControllerKeys() : GameController( 0, MODE_DIRECTIONAL ) { }
  
  
  GameControllerKeys(int id_, VirtualKey up_ = fabgl::VK_NONE, VirtualKey down_ = fabgl::VK_NONE, 
                      VirtualKey left_ = fabgl::VK_NONE, VirtualKey right_ = fabgl::VK_NONE, 
                      VirtualKey buttonA_ = fabgl::VK_NONE, VirtualKey buttonB_ = fabgl::VK_NONE)
                      : GameController( id_, MODE_DIRECTIONAL )
  {
    up = up_; down = down_; left = left_; right = right_; buttonA = buttonA_; buttonB = buttonB_;

    auto keyboard = PS2Controller.keyboard();      
    active = keyboard->isKeyboardAvailable(); // if keyboard detected
  }
  
  VirtualKey up,down,left,right,buttonA,buttonB;

  bool getKeyStatus( VirtualKey key )
  {
    if ( !active || key == fabgl::VK_NONE) 
      return false;

    auto keyboard = PS2Controller.keyboard();      
    return keyboard->isVKDown(key);
  }

  virtual bool isLeft() { return getKeyStatus(left); }
  virtual bool isRight() { return getKeyStatus(right); }
  virtual bool isUp() { return getKeyStatus(up); }
  virtual bool isDown() { return getKeyStatus(down); }
  virtual bool isButtonA() { return getKeyStatus(buttonA); }
  virtual bool isButtonB() { return getKeyStatus(buttonB); }
  
    
};

struct GameControllerJoystick : public GameController
{
  GameControllerJoystick() : GameController( 0, MODE_DIRECTIONAL ) { }
  
  GameControllerJoystick( int id_, int pinUp_ = -1, int pinDown_ = -1, int pinLeft_ = -1, int pinRight_ = -1, 
                            int pinButtonA_ = -1, int pinButtonB_ = -1) 
                            : GameController( id_, MODE_DIRECTIONAL )
  {
     if( pinUp_ != -1 )       { pinUp = pinUp_;           pinMode( pinUp_, INPUT_PULLDOWN);  }
     if( pinDown_ != -1 )     { pinDown = pinDown_;       pinMode( pinDown_, INPUT_PULLDOWN);  }
     if( pinLeft_ != -1 )     { pinLeft = pinLeft_;       pinMode( pinLeft_, INPUT_PULLDOWN);  }
     if( pinRight_ != -1 )    { pinRight = pinRight_;     pinMode( pinRight_, INPUT_PULLDOWN);  }
     if( pinButtonA_ != -1 )  { pinButtonA = pinButtonA_; pinMode( pinButtonA_, INPUT_PULLDOWN);  }
     if( pinButtonB_ != -1 )  { pinButtonB = pinButtonB_; pinMode( pinButtonB_, INPUT_PULLDOWN);  }     
  }
  
  int pinUp = -1;
  int pinDown = -1;
  int pinLeft = -1;
  int pinRight = -1;
  int pinButtonA = -1;
  int pinButtonB = -1;

  virtual bool isLeft()     { return digitalRead(pinLeft); }
  virtual bool isRight()    { return digitalRead(pinRight); }
  virtual bool isUp()       { return digitalRead(pinUp); }
  virtual bool isDown()     { return digitalRead(pinDown); }
  virtual bool isButtonA()  { return digitalRead(pinButtonA); }
  virtual bool isButtonB()  { return digitalRead(pinButtonB); }
};


struct GameControllerMouse : public GameController
{
  GameControllerMouse() : GameController( 0, MODE_DIRECTIONAL ) { }
  
    GameControllerMouse( int id_) : GameController( id_, MODE_RELATIVEPOS )
    {
      auto mouse = PS2Controller.mouse();
      active = mouse->isMouseAvailable(); // activate if mouse found
      if (active)
      {
          auto mouse = PS2Controller.mouse();
          mouse->setSampleRate( 100 );
          mouse->setResolution( 2 );
          mouse->setScaling( 1 );
      }
    }

    bool bLeftButton = false;
    bool bRightButton = false;
    bool bMiddleButton = false;

    int dx = 0;
    int dy = 0;

    virtual void update()
    {
      if (active )
      {    
        auto mouse = PS2Controller.mouse();
          
         while (mouse->deltaAvailable()) 
         {
            MouseDelta mouseDelta;
            mouse->getNextDelta(&mouseDelta);
        
            bLeftButton= mouseDelta.buttons.left == 1;
            bRightButton= mouseDelta.buttons.right == 1;
            bMiddleButton= mouseDelta.buttons.middle == 1;
            
            dx += mouseDelta.deltaX;        
            dy += mouseDelta.deltaY;     
         }   
      }
    }
    
    virtual bool isUp()  { return bLeftButton; }
    virtual bool isDown()  { return bRightButton; }
    virtual bool isButtonA()  { return bLeftButton; }
    virtual bool isButtonB()  { return bRightButton; }

    int getDX() { int v=dx; dx=0; return v; }
    int getDY() { int v=dy; dy=0; return v; }
};
