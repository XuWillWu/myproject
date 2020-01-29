

#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;
 
/*
 * Global game state variables
 */
const int Border = 1;
const int BufferSize = 10;
int FPS = 30;
int frames = 0;
int handle_times = 0;
const int width = 800;
const int height = 600;
int turn = 0;
int gameover = 0;
int score = 0;
int speed = 5;
int start = 0;
int snake_pause = 0;
int restart = 0;



/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
	GC		 gc[3];
	int		width;		// size of window
	int		height;
};

struct unit {
    int x_u;
    int y_u;
};
/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}
//

 

/*
 * An abstract class representing displayable things. 
 */
class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};


class Fruit : public Displayable {
public:
    virtual void paint(XInfo &xinfo) {
            XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[color], x, y, 20, 20);
    }
    
    Fruit() {
        // ** ADD YOUR LOGIC **
        // generate the x and y value for the fruit
        x = 50;
        y = 50;
        color = 2;
    }
    int getX() {
        return x;
    }
    
    int getY() {
        return y;
    }
    
    int getcolor() {
        return color;
    }
    // ** ADD YOUR LOGIC **
    /*
     * The fruit needs to be re-generated at new location every time a snake eats it. See the assignment webpage for more details.
     */
    void generate_fruit(int a,int b){
        int app = 0;
         while(app == 0){
             x = rand()% 640;
             y = rand()% 580;
             int z = rand()% 100;
             if(z < 10){
                 color = 3;
             }else if(z < 30){
                 color = 4;
             }else if(z > 30){
                 color = 2;
             }
            if(a == x && b == y){
                app = 0;
            }else{
                app = 1;
            }
         }
     }
    
private:
    int x;
    int y;
    int color;
};

Fruit fruit;

class Snake : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
            for(int i = 0; i < length; i++){
                XDrawRectangle(xinfo.display, xinfo.window, xinfo.gc[5], slist[i].x_u, slist[i].y_u, blockSize, blockSize);
            }
		}
		
		void move(XInfo &xinfo) {
            if(turn == 0){
                switch(toward){
                    case 1:
                        y = y - blockSize;
                        break;
                    case 2:
                        x = x + blockSize;
                        break;
                    case 3:
                        y = y + blockSize;
                        break;
                    case 4:
                        x = x - blockSize;
                        break;
                }
            }else if(turn == 1){//left
                if(toward == 1 || toward == 3){
                    x = x - blockSize;
                    toward = 4;
                }else if(toward == 2){
                    x = x + blockSize;
                }else{
                    x = x - blockSize;
                }
            }else if(turn == 2){//right
                if(toward == 1 || toward == 3){
                    x = x + blockSize;
                    toward = 2;
                }else if(toward == 2){
                    x = x + blockSize;
                }else{
                    x = x - blockSize;
                }
            }else if(turn == 3){//up
                if(toward == 2 || toward == 4){
                    y = y - blockSize;
                    toward = 1;
                }else if(toward == 1){
                    y = y - blockSize;
                }else{
                    y = y + blockSize;
                }
            }else if(turn == 4){//up
                if(toward == 2 || toward == 4){
                    y = y + blockSize;
                    toward = 3;
                }else if(toward == 1){
                    y = y - blockSize;
                }else{
                    y = y + blockSize;
                }
            }
            turn = 0;
            
			
            
        

            // ** ADD YOUR LOGIC **
            // Here, you will be performing collision detection between the snake, 
            // the fruit, and the obstacles depending on what the snake lands on.
		}
		
		int getX() {
			return x;
		}
		
		int getY() {
			return y;
		}

        /*
         * ** ADD YOUR LOGIC **
         * Use these placeholder methods as guidance for implementing the snake behaviour. 
         * You do not have to use these methods, feel free to implement your own.
         */
    
        bool collision(int a,int b){
            switch(toward){
                case 1:
                    if(((a >= x && a <= x+blockSize) && (b >= y && b <= slist[0].y_u+blockSize ))||
                       ((a+blockSize >= x && a <= x) && (b >= y && b <= slist[0].y_u+blockSize))||
                       ((a >= x && a <= x+blockSize) && (b+blockSize >= y && b <= slist[0].y_u))||
                       ((a+blockSize >= x && a <= x) && (b+blockSize >= y && b <= slist[0].y_u))){
                        return true;
                    }
                    break;
                case 2:
                    if(((a >= slist[0].x_u && a <= x+blockSize) && (b >= y && b <= y+blockSize))||
                       ((a+blockSize >= slist[0].x_u && a <= x) && (b >= y && b <= y+blockSize))||
                       ((a >= slist[0].x_u && a <= x+blockSize) && (b+blockSize >= y && b <= y))||
                       ((a+blockSize >= slist[0].x_u && a <= x) && (b+blockSize >= y && b <= y))){
                        return true;
                    }
                    break;
                case 3:
                    if(((a >= x && a <= x+blockSize) && (b <= y + blockSize && b >= slist[0].y_u))||
                       ((a+blockSize >= x && a <= x) && (b <= y + blockSize && b >= slist[0].y_u))||
                       ((a >= x && a <= x+blockSize) && (b <= y  && b+blockSize >= slist[0].y_u))||
                       ((a+blockSize >= x && a <= x) && (b <= y && b+blockSize >= slist[0].y_u))){
                        return true;
                    }
                    break;
                case 4:
                    if(((a >= x && a <= slist[0].x_u+blockSize) && (b >= y && b <= y+blockSize))||
                       ((a +blockSize >= x && a <= slist[0].x_u) && (b >= y && b <= y+blockSize))||
                       ((a >= x && a <= slist[0].x_u+blockSize) && (b+blockSize >= y && b <= y))||
                       ((a+blockSize >= x && a <= slist[0].x_u) && (b+blockSize >= y && b <= y))){
                        return true;
                    }
                    break;
            }
        }
        void didEatFruit(int a,int b,int c) {
            int eat = 0;
            if(collision(a,b)){
                eat = 1;
            }
            if(eat == 1){
                struct unit eaten_fruit;
                eaten_fruit.x_u = slist[length-1].x_u;
                eaten_fruit.y_u = slist[length-1].y_u;
                slist.push_back(eaten_fruit);
                length++;
                if(c == 3){
                    score += 9;
                }
                if(c == 4){
                    score += 4;
                }
                score++;
                eat = 0;
                fruit.generate_fruit(x,y);
            }
        }

        void didHitObstacle() {
            if(toward == 1 || toward == 4){
                if (x < 0 || y < 0 ){
                    gameover = 1;
                }
            }else{
                if (x+blockSize > 640 || y+blockSize > height){
                    gameover = 1;
                }
            }
            for(int i = 1; i < length; i++){
                int a = slist[i].x_u;
                int b = slist[i].y_u;
                if(x == a && y == b){
                    gameover = 1;
                }
            }
        }
    
        void update() {
            for(int i = length-1; i > 0; i--){
                slist[i].x_u = slist[i-1].x_u;
                slist[i].y_u = slist[i-1].y_u;
            }
            slist[0].x_u = x;
            slist[0].y_u = y;
        }
    
    void restart(){
        for(int i = 0; i < length; i++){
            slist.pop_back();
        }
        blockSize = 20;
        toward = 2;//1 N 2 E 3 S 4 W
        length = 3;
        x = 100;
        y = 450;
        score = 0;
        for(int i = 0; i < length; i++){
            struct unit u;
            u.x_u = x - i*blockSize;
            u.y_u = y;
            slist.push_back(u);
            //slist.push_back(&u);
        }
    }
		
		Snake(int x, int y, int z): x(x), y(y) {
            blockSize = 20;
            toward = z;//1 N 2 E 3 S 4 W
            length = 3;
            for(int i = 0; i < length; i++){
                struct unit u;
                u.x_u = x - i*blockSize;
                u.y_u = y;
                slist.push_back(u);
                //slist.push_back(&u);
            }
		}
	
	private:
		int x;
		int y;
		int blockSize;
        int toward;//head to
        int length;//mult of fruit
        vector<unit> slist;
        //vector<unit *> slist;
    
};



list<Displayable *> dList;           // list of Displayables
Snake snake(100, 450, 2);



/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
    Colormap screen_colormap;
    
    XColor red, brown, blue, yellow, green;
    if(argc == 3){
        std::string frames = argv[1];
        FPS = stoi(frames);
        std::string v = argv[2];
        speed = stoi(v);
        
    }
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   /*
	* Find out some things about the display you're using.
	*/
	xInfo.screen = DefaultScreen( xInfo.display );
    
    screen_colormap = DefaultColormap(xInfo.display, DefaultScreen(xInfo.display));
    
    XAllocNamedColor(xInfo.display, screen_colormap, "green", &green, &green);
    XAllocNamedColor(xInfo.display, screen_colormap, "red", &red, &red);
    XAllocNamedColor(xInfo.display, screen_colormap, "brown", &brown, &brown);
    XAllocNamedColor(xInfo.display, screen_colormap, "blue", &blue, &blue);
    XAllocNamedColor(xInfo.display, screen_colormap, "yellow", &yellow, &yellow);

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 800;
	hints.height = 600;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"animation",		// window's title
		"Animate",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);
    XFontStruct * font;
    font = XLoadQueryFont (xInfo.display, "9x15bold");
    XSetFont (xInfo.display, xInfo.gc[i], font->fid);
    
    i = 1;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], green.pixel);
    XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
    XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
    XSetLineAttributes(xInfo.display, xInfo.gc[i],
                       1, LineSolid, CapButt, JoinRound);
    i = 2;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], green.pixel);
    XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
    
    i = 3;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], red.pixel);
    XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
    
    i = 4;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], yellow.pixel);
    XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
    
    i = 5;
    xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
    XSetForeground(xInfo.display, xInfo.gc[i], brown.pixel);
    XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));

	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | 
		PointerMotionMask | 
		EnterWindowMask | LeaveWindowMask |
		StructureNotifyMask);  // for resize events

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	XFlush(xInfo.display);
}

/*
 * Function to repaint a display list
 */
void repaint( XInfo &xinfo) {
	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	XClearWindow( xinfo.display, xinfo.window );
	
	// get height and width of window (might have changed since last repaint)

	//XWindowAttributes windowInfo;
	//XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	//unsigned int height = windowInfo.height;
	//unsigned int width = windowInfo.width;

	// big black rectangle to clear background
    
	// draw display list
	while( begin != end ) {
		Displayable *d = *begin;
		d->paint(xinfo);
		begin++;
	}
    std::string text("score:");
    std::string pts(to_string(score));
    std::string text_0("RED fruit:");//3
    std::string text_0_1("10 pts");
    std::string text_1("YELLOW fruit:");//4
    std::string text_1_1("5 pts");
    std::string text_2("GREEN fruit:");//2
    std::string text_2_1("1 pts");
    std::string text_3("RESTART:");
    std::string text_3_1("enter");
    std::string text_4("QUIT:");
    std::string text_4_1("q");
    std::string text_5("PAUSE:");
    std::string text_5_1("p");
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],670, 15, text.c_str(), text.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],670, 30, pts.c_str(), pts.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[3],670, 60, text_0.c_str(), text_0.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[3],670, 75, text_0_1.c_str(), text_0_1.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[4],670, 105, text_1.c_str(), text_1.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[4],670, 120, text_1_1.c_str(), text_1_1.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[2],670, 150, text_2.c_str(), text_2.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[2],670, 165, text_2_1.c_str(), text_2_1.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],670, 195, text_3.c_str(), text_3.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],670, 210, text_3_1.c_str(), text_3_1.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],670, 240, text_4.c_str(), text_4.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],670, 255, text_4_1.c_str(), text_4_1.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],670, 285, text_5.c_str(), text_5.length());
    XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],670, 300, text_5_1.c_str(), text_5_1.length());
    XDrawLine(xinfo.display, xinfo.window, xinfo.gc[0], 660, 0, 660, 600);
	XFlush( xinfo.display );
}



void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			error("Terminating normally.");
        }else if(text[0] == 'p'){
            if (snake_pause == 0){
                snake_pause = 1;
            }else{
                snake_pause = 0;
            }
        }
	}
    switch(key){
        case XK_Left:
            turn = 1;
            break;
        case XK_Right:
            turn = 2;
            break;
        case XK_Up:
            turn = 3;
            break;
        case XK_Down:
            turn = 4;
            break;
        case XK_Return:
            if(start  == 1){
                restart = 1;
                gameover = 0;
            }
            if(start == 0){
                start = 1;
            }
            
            break;
    }
}

void paint( XInfo &xinfo) {//print window and basic info
    XEvent event;
    while( start == 0 ) {
        std::string text("name: Xu Wu  id:20598898");
        std::string ins("use arrow keys to control the snake, return to (re)start, p to pause,q to quit");
        XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],300, 300, text.c_str(), text.length());
        XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],80, 320, ins.c_str(), ins.length());
        if (XPending(xinfo.display) > 0) {//# of event received
            XNextEvent( xinfo.display, &event );//copy first event
            cout << "event.type=" << event.type << "\n";
            switch( event.type ) {
                case KeyPress:
                    handleKeyPress(xinfo, event);
                    break;
            }
        }
        usleep(1000000 / FPS);
        XFlush( xinfo.display );
    }
    
}
unsigned long now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}


void handleAnimation(XInfo &xinfo, int inside) {
    /*
     * ADD YOUR OWN LOGIC
     * This method handles animation for different objects on the screen and readies the next frame before the screen is re-painted.
     */
    
    int handle_times_tot = frames * speed / FPS;
    //cout << "ppp"<<frames <<" "<< speed<<" "<<FPS << endl;
    while(handle_times_tot > handle_times){
        //cout << "ppp"<<handle_times_tot << handle_times << endl;
        handle_times++;
        int a = snake.getX();
        int b = snake.getY();
        int c = fruit.getX();
        int d = fruit.getY();
        int e = fruit.getcolor();
        
        
        
        if(restart == 1){
            restart = 0;
            snake.restart();
            fruit.generate_fruit(snake.getX(),snake.getY());
        }
        snake.move(xinfo);
        snake.didHitObstacle();
        if(((a-c)*(a-c) + (b-d)*(b-d)) < 1600){
            snake.didEatFruit(c,d,e);
        }
        snake.update();
    }
    
    
    
    
}

// get microseconds


void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	dList.push_front(&snake);
    dList.push_front(&fruit);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;

	while( true ) {
		/*
		 * This is NOT a performant event loop!  
		 * It needs help!
		 */
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			cout << "event.type=" << event.type << "\n";
			switch( event.type ) {
				case KeyPress:
					handleKeyPress(xinfo, event);
					break;
				case EnterNotify:
					inside = 1;
					break;
				case LeaveNotify:
					inside = 0;
					break;
			}
		}
        unsigned long end = now();    // get time in microsecond
        
        
        if (end - lastRepaint > 500000 / FPS) {//speed
            // clear background
            XClearWindow(xinfo.display, xinfo.window);
            frames++;
            handleAnimation(xinfo, inside);
            repaint(xinfo);//change speed
            XFlush( xinfo.display );
            
            lastRepaint = now();
        }
        if (XPending(xinfo.display) == 0) {
            usleep(500000 / FPS - (end - lastRepaint));
        }
		
        while(snake_pause == 1){
            std::string text("pause");
            XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],350, 300, text.c_str(), text.length());
            if (XPending(xinfo.display) > 0) {
                XNextEvent( xinfo.display, &event );
                //cout << "event.type=" << event.type << "\n";
                switch( event.type ) {
                    case KeyPress:
                        handleKeyPress(xinfo, event);
                        break;
                }
            }
            usleep(1000000/FPS);
            XFlush( xinfo.display );
        }
        
        while(gameover == 1){
            std::string text("GAME OVER, press return to restart");
            XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],250, 300, text.c_str(), text.length());
            if (XPending(xinfo.display) > 0) {
                XNextEvent( xinfo.display, &event );
                cout << "event.type=" << event.type << "\n";
                switch( event.type ) {
                    case KeyPress:
                        handleKeyPress(xinfo, event);
                        break;
                }
            }
            usleep(1000000/FPS);
            XFlush( xinfo.display );
        }
	}
    
}


/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo;

	initX(argc, argv, xInfo);
    paint(xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
