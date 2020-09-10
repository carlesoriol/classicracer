#define MENU_TIMEOUT 30000L

#define LEFT_POS      64
#define RIGHT_POS     228
#define MIDDLE_POS    104

#define LEFT 0
#define RIGHT 1


int playercontrol[2] = {0,0};


struct Menu : public Scene 
{
  Menu()
    : Scene(0, 20, VGAController.getViewPortWidth(), VGAController.getViewPortHeight())
  {
  }
  

  Bitmap bitmap_caricon;
  long menulastactivity = 0;     
  int exitvalue = -1;
  char *textControls[6] = { ".......", " MOUSE ", "JOYSTICK", " ARROWS ", "A S T F ", "Q A O P" };
  bool lockedControls[4] = { false, false, false, false };
    
  void init()  
  {
    menulastactivity = millis();
    
    canvas.selectFont(&fabgl::FONT_8x8);
    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));

    Bitmap bitmap_classicracer = Bitmap(21*8, 16, bitmap_classicracer_data, PixelFormat::Mask, RGB888(255, 255, 255));   
    bitmap_caricon = Bitmap(8, 8, bitmap_caricon_data, PixelFormat::Mask, RGB888(128, 128, 0));
   
    canvas.setBrushColor(RGB888(0, 0xff, 0));
    canvas.clear();

    canvas.drawBitmap( 10*8, 2*8, &bitmap_classicracer );    
    canvas.setPenColor(RGB888(0xFF, 0xFF, 0));
    canvas.drawText(7*8, 5*8, "VIDEO COMPUTER SYSTEM");     
    canvas.drawText(5*8, 7*8, "GAME PROGRAM");     

    canvas.drawText(4*8, 19*8, "MOUSE JOYSTICK ARROWS ASTF QAOP");      
    
    canvas.setPenColor(RGB888(64, 64, 0));     
    canvas.drawText(16, 23*8, "A NEW FANWARE BY CARLES ORIOL - 2020");     
  
    canvas.waitCompletion();
  }



  void setControl( int direction, int control) // direction LEFT or RIGHT
  {
    if( !lockedControls[control-1] ) // debouncer
    {    
      if( direction == RIGHT) 
        if( playercontrol[LEFT] == control) { playercontrol[LEFT] = 0; playPic(); }
        else { playercontrol[RIGHT] = control; playPong(); }
      else
        if( playercontrol[RIGHT] == control) { playercontrol[RIGHT] = 0; playPic(); }
        else { playercontrol[LEFT] = control; playPong(); } 
  
        menulastactivity = millis();
     
      lockedControls[control-1] = true;
    }
  }

  void update( int updateCount )
  {    
    int t= (updateCount/3) % 8; t = (t>4?8-t:t); // Bouncer
    
    canvas.setBrushColor(RGB888(0, 0xff, 0));
    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
    canvas.setPenColor(RGB888(0xff, 0xff, 0xff));
    //canvas.fillRectangle(0, 13*8, 299, 14*8-1); 
    //canvas.drawText(MIDDLE_POS-16-(( playercontrol[0] == 0 && playercontrol[1] == 0)?t:0), 10*8, " SELECT CONTROLS ");          
    canvas.drawText(MIDDLE_POS-16, 10*8, " SELECT CONTROLS ");          
    canvas.setPenColor(RGB888(0xff, 0xff, 0x00));
    canvas.drawText(LEFT_POS-3*8, 12*8, "LEFT PLAYER");          
    canvas.drawText(RIGHT_POS-5*8, 12*8, "RIGHT PLAYER");          

    canvas.setPenColor(RGB888(0xff, 0, 0));
    canvas.fillRectangle(0, 14*8, 299, 15*8-1);
    canvas.drawText(LEFT_POS-1*8, 14*8, textControls[playercontrol[LEFT]]);
    canvas.drawText(RIGHT_POS-3*8, 14*8, textControls[playercontrol[RIGHT]]);

    canvas.setGlyphOptions(GlyphOptions().FillBackground(true));
    
    if( playercontrol[0] != 0 || playercontrol[1] != 0)
    {
      canvas.setPenColor(RGB888(0xff, 0, 0));
      canvas.drawText(MIDDLE_POS-32+t, 17*8, " ACCELERATE TO START ");              
    }
    else
    {
      canvas.setPenColor(RGB888(0xff, 0xff, 0xff));
      canvas.drawText(MIDDLE_POS-28+t, 17*8, " MOVE LEFT OR RIGHT  ");          
    }
        
    canvas.waitCompletion();
        
    bool bStartRace = false;
    bool bExitB = false;

    for( int ncont = 0; ncont < sizeof(gameControllers) / sizeof( GameController *); ncont++)
    {
      GameController *controller = gameControllers[ncont];
      if( controller->mode == MODE_DIRECTIONAL)
      {
        if( controller->isRight() ) setControl( RIGHT, controller->id);
        else if( controller->isLeft() )  setControl( LEFT,  controller->id);
        else lockedControls[controller->id-1] = false;
      }
      else if( controller->mode == MODE_RELATIVEPOS)
      {
        controller->update();    
        int dx = controller->getDX();
        if( dx >  20) setControl( RIGHT, controller->id);      
        else if( dx < -20) setControl( LEFT, controller->id);      
        else lockedControls[cMouse.id-1] = false;           // release debouncer      
      }

      if( controller->isButtonB() ) bExitB = true;
    }

    if( gameControllers[playercontrol[LEFT]]->isButtonA() || 
        gameControllers[playercontrol[LEFT]]->isUp() || 
        gameControllers[playercontrol[RIGHT]]->isButtonA() || 
        gameControllers[playercontrol[RIGHT]]->isUp()       
        )
    {
      exitvalue = 1;
      this->stop();  
      return;    
    }
        
    if ( millis() > menulastactivity + MENU_TIMEOUT || bExitB )
    {
          exitvalue = 2;
          this->stop();    
          return;  
    }
              
  }

  void collisionDetected(Sprite *spriteA, Sprite *spriteB, Point collisionPoint  ) {}

};
