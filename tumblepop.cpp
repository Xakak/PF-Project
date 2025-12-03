#include <iostream>
#include <fstream>
#include <cmath>
#include<unistd.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>

using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;

void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{

			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
	}

}
}

void platform_collision_y(char** lvl,float& offset_x,float& speed_x,float &postion_x,float& position_y,const int cell_size,int& height,int &width){
	if (speed_x == 0) return;
	offset_x = postion_x;
	offset_x += speed_x;

	char left_up = lvl[(int)(position_y + 10) / cell_size][(int)(offset_x) / cell_size];
	char left_mid = lvl[(int)(position_y + height/2) / cell_size][(int)(offset_x) / cell_size];
	char left_down = lvl[(int)(position_y + height - 10) / cell_size][(int)(offset_x) / cell_size];

	char right_up = lvl[(int)(position_y + 10) / cell_size][(int)(offset_x + width) / cell_size];
	char right_mid = lvl[(int)(position_y + height /2) / cell_size][(int)(offset_x + width) / cell_size];
	char right_down = lvl[(int)(position_y + height - 10) / cell_size][(int)(offset_x + width) / cell_size];

	if (speed_x > 0){
		if(right_up == '#' || right_mid == '#' || right_down == '#')
			speed_x = 0;
	}
	else if(speed_x < 0){
		if ( left_down == '#' || left_up == '#' || left_mid == '#'){
			speed_x = 0;
		}
	}
	

}


void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth,bool isfacingleft)
{
    offset_y = player_y;
	offset_y += velocityY;
	int x = 0;
	if(!isfacingleft){//if player is facing right then it means the hitbox is diff from visual so we manually adjust the collision
		x = 100;
	}
    onGround = false;
	char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x-x) / cell_size];
    char bottom_right_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x-x + Pwidth) / cell_size];
    char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x -x+ Pwidth / 2) / cell_size];

    if (velocityY >= 0)//if going down check for collision and above platform
    {
        // Calculate the grid row where the feet are currently(taken from above)
        int feet_row = (int)(offset_y + Pheight) / cell_size;
        
        if (bottom_left_down == '#' || bottom_right_down == '#' || bottom_mid_down == '#')//if below is platform
        {
            float platform_top = feet_row * cell_size;//same as feet_row because feet is on platform
            float old_feet_y = player_y + Pheight;

            if (old_feet_y <= platform_top + 5) //if sprite was above the platform ( the lower we go the y coordinate increases) 
            {
                player_y = platform_top - Pheight;//teleports to top of platform to avoid sticking in middle  {player_y is the top left corner of sprite hitbox so subtract
				// P_height to appear at top, in y axis subtract means to go up}
				
                velocityY = 0;
                onGround = true;
                return; // Exit function so it doesnot go down
            }
        }
    }
	
	//if going up ignore collision
    player_y = offset_y;
    velocityY += gravity;
    if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
		

}


bool checkcollision(float x1, float y1, float w1, float h1, float x2,float y2, float w2, float h2,float speed1,float speed2){
	//adjust the hitbox
	if (speed1 > 0){
		x1 -= w1;
	}
	if (speed2 > 0){
		x2 -= w2;
	}
	y2 += 40;//shifting the position of ghost a bit down to avoid unnecessary collision while jumping

	//top left corner
	if(x1 >= x2 && x1<= x2+w2 && y1 >= y2 && y1 <= y2+h2){
		return true;
	}

	//top middle point
	if (x1 + w1/2 >= x2 && x1+w1/2 <= x2+w2 && y1 >= y2&& y1 <= y2+h2){
		return true;
	}

	//top right corner
	if(x1 + w1 >= x2 && x1+w1 <= x2+w2 && y1 >= y2 && y1 <= y2+h2){
		return true;
	}

	//left middle point
	if(x1>=x2 && x1 <= x2 + w2 && y1+h1/2 >= y2 && y1+h1/2 <= y2+h2){
		return true;
	}

	//right middle point
	if(x1 + w1 >= x2 && x1+w1 <= x2+w2 && y1+h1/2 >= y2 && y1+h1/2 <= y2+h2){
		return true;
	}

	//bottom left corner
	if(x1 >= x2 && x1<= x2+w2 && y1+h1 >= y2 && y1+h1 <= y2+h2){
		return true;
	}

	//bottom middle poiint
	if (x1 + w1/2 >= x2 && x1+w1/2 <= x2+w2 && y1+h1 >= y2&& y1+h1 <= y2+h2){
		return true;
	}

	//bottom right corner
	if(x1 + w1 >= x2 && x1+w1 <= x2+w2 && y1+h1 >= y2 && y1+h1 <= y2+h2){
		return true;
	}

	return false;
}


void jump(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	velocityY -= 22*gravity;
}

//sucking mechanism
//when an enemy comes in the range of the vacuum then it will start being sucked and when it touches the player then it will disappear and go into the bag
void suck(float speed,float& enemy_x,float &enemy_y,int enemy_w,int enemy_h, float e_speed,int player_x,int player_y,int pwidth, int pheight,Sprite& enemysprite,bool &isenemyalive){
	if(Keyboard::isKeyPressed(Keyboard::A)){
		if (checkcollision(player_x-15,player_y,pwidth,pheight,enemy_x,enemy_y,enemy_w,enemy_h,speed,e_speed))
		isenemyalive = false;
		enemy_x += 10;
		
	}

	else if (Keyboard::isKeyPressed(Keyboard::D)){
		if (checkcollision(player_x+15,player_y,pwidth,pheight,enemy_x,enemy_y,enemy_w,enemy_h,speed,e_speed))
		isenemyalive = false;
		enemy_x -= 10;
	}

	else if (speed > 0){
		if (checkcollision(player_x+15,player_y,pwidth,pheight,enemy_x,enemy_y,enemy_w,enemy_h,speed,e_speed))
		isenemyalive = false;
		enemy_x -= 10;
	}

	else if (speed < 0){
		if (checkcollision(player_x-15,player_y,pwidth,pheight,enemy_x,enemy_y,enemy_w,enemy_h,speed,e_speed))
		isenemyalive = false;
		enemy_x += 10;
	}
	else if (Keyboard::isKeyPressed(Keyboard::W)){
		if (checkcollision(player_x,player_y - 15,pwidth,pheight,enemy_x,enemy_y,enemy_w,enemy_h,speed,e_speed))
		isenemyalive = false;
		enemy_y += 10;
	}

	else if (Keyboard::isKeyPressed(Keyboard::S)){
		if (checkcollision(player_x,player_y+15,pwidth,pheight,enemy_x,enemy_y,enemy_w,enemy_h,speed,e_speed))
		isenemyalive = false;
		enemy_y -= 10;
	}


	
	
}


void moveright(float &player_x,float& speed,Sprite& playerSprite,int& frame,int& timer){
	if(speed < 0){
		speed *= -1;
	}
	player_x += speed;
	if (player_x == 1150){
		player_x = 0;
	}
	timer++;
	if (timer > 8){
	playerSprite.setTextureRect(IntRect(317-(32*frame),36,32,45));
	if (frame > 2)
		frame = 0;
	frame++;
	timer=0;
	}

}

void moveleft(float &player_x,float& speed,Sprite& playerSprite,int& frame, int& timer){
	if (speed > 0){
		speed *= -1;
	}
	player_x += speed;
	if (player_x == 0){
		player_x = 1150;
	}
	timer++;
	playerSprite.setScale(3,3);
	if (timer > 8){
	playerSprite.setTextureRect(IntRect(317-(32*frame),36,32,45));
	if (frame > 2)
		frame = 0;
	frame++;
	timer=0;
	}
}

void ghosts(float ghost_x[],int ghost_speed[],int n,Sprite ghostsprite[],bool isfacingleft[],int ghost_state[],int ghost_timer[]){
	//n are the no of ghost.

	for (int i = 0 ; i < n; i++){
	
	ghost_timer[i]--;
	if(ghost_timer[i]<=0){
		if(ghost_state[i]==1){//is moving
			ghost_state[i]=0;
			ghost_timer[i] = rand()%60+30;//wait between 0.5 and 1.5 seconds
		}
		else{
			ghost_state[i]=1;
			ghost_timer[i]=rand()%180+300;//walk btween 3 to 5
			if( rand()%10 + 2){ //generates a random no beteen 2 and 10 and 50/50 chance of odd/even
				ghost_speed[i] *=	-1;
			}
			
		}
	}
	if (ghost_state[i]==1){
	if (ghost_x[i] >= 1130){
		ghost_speed[i] *= -1;
	}

	if (ghost_x[i] <= 0){
		ghost_speed[i] *= -1;
	}
	
	if (ghost_speed[i]< 0){
		if (!isfacingleft[i] ){
			ghost_x[i] -= 96;
		isfacingleft[i] = true;
		}
		ghostsprite[i].setScale(3,3);
		

	}
	if (ghost_speed[i]> 0){
		if (isfacingleft[i] == true ){
			ghost_x[i] += 96;
		isfacingleft[i] = false;
		}
		ghostsprite[i].setScale(-3,3);
			
	}
	

	
	
	ghost_x[i] += ghost_speed[i];}
	
}
}

void skeletons(float skeleton_x[],float skeleton_y[],int skeleton_speed[],int n,Sprite skeletonSprite[],bool isfacingleft[],int skeleton_state[],int skeleton_timer[],int cell_size){
	//n are the no of skeletons.
	//for level one
	float upperPlatform = 3*cell_size - 225;
	float middlePlatform = 9*cell_size - 225;
	float floor = 13*cell_size -225;
	float platform_left = 3*cell_size;
	float platform_right = 15*cell_size;

	for (int i = 0 ; i < n; i++){
	
	skeleton_timer[i]--;
	if(skeleton_timer[i]<=0){

		if((rand()%10)<3 && skeleton_state[i] == 1){//chance is 30%
			if(skeleton_y[i] == floor){
				skeleton_y[i] = middlePlatform;
				if (skeleton_x[i] < platform_left) skeleton_x[i] = platform_left;
				if ( skeleton_x[i] > platform_right) skeleton_x[i] = platform_right;
			}
			if(skeleton_y[i] == middlePlatform){
				if(rand()%2==0)// 50 % chance of up or down
					skeleton_y[i] = upperPlatform;
				else
					skeleton_y[i] = floor;
				if (skeleton_x[i] < platform_left) skeleton_x[i] = platform_left;
				if ( skeleton_x[i] > platform_right) skeleton_x[i] = platform_right;
			}
			if(skeleton_y[i] == upperPlatform){
				skeleton_y[i] = middlePlatform;
				if (skeleton_x[i] < platform_left) skeleton_x[i] = platform_left;
				if ( skeleton_x[i] > platform_right) skeleton_x[i] = platform_right;
			}

			

		}
		else{
		if(skeleton_state[i]==1){//is moving
			skeleton_state[i]=0;
			skeleton_timer[i] = rand()%60+30;//wait between 0.5 and 1.5 seconds
		}
		else{
			skeleton_state[i]=1;
			skeleton_timer[i]=rand()%180+300;//walk btween 3 to 5
			
		}
	}
}
	if (skeleton_state[i]==1){
	if (skeleton_x[i] >= 1130){
		skeleton_speed[i] *= -1;
	}

	if (skeleton_x[i] <= 0){
		skeleton_speed[i] *= -1;
	}
	
	if (skeleton_speed[i]< 0){
		if (!isfacingleft[i] ){
			skeleton_x[i] -= 96;
		isfacingleft[i] = true;
		}
		skeletonSprite[i].setScale(3,3);
		

	}
	if (skeleton_speed[i]> 0){
		if (isfacingleft[i] == true ){
			skeleton_x[i] += 96;
		isfacingleft[i] = false;
		}
		skeletonSprite[i].setScale(-3,3);
			
	}
	

	
	
	skeleton_x[i] += skeleton_speed[i];}
	
}
}

bool onplatform(char **lvl,float width, float height,float posx, float posy, const int cell_size,int speed){
	float offset = posx + speed;
	char bottomleft = lvl[((int)(posy + height)/cell_size)][(int)(offset)/cell_size];
	char bottommiddle = lvl[((int)(posy + height)/cell_size)][(int)(offset + width/2)/cell_size];
	char bottomright = lvl[((int)(posy + height)/cell_size)][(int)(offset+width)/cell_size];

	if (bottomleft == '#'){//only check for bottom left since we added the width in ghost func so it is the left bottom of sprite.
		return true;
	}

	return false;

}



void playerdies(Sprite &playersprite,int& frame,int& timer){
	timer++;
	if(timer>10){
		playersprite.setTextureRect(IntRect(19+(frame*32+frame*15),85,32,34));
		
		frame++;
		//frame is not reset to zero so that it does not repeat the animation
		timer = 0;
	}

}

void getvacuum(Sprite& vacuumsprite,Sprite& vacupsprite,float& player_x,float& player_y,int& frame,int& timer,float &speed,float& vac_x,float& vac_y){

if(Keyboard :: isKeyPressed(Keyboard::A) || Keyboard :: isKeyPressed(Keyboard::D) ||  Keyboard :: isKeyPressed(Keyboard::Space)){
	timer++;
	if (timer > 0){//height and width of three frames are different so we manually check all frames instead of general formula
		timer = 0; //reset the timer
		if (frame > 2){
		frame = 0;//reset it to 0 so that the animation keeps repeating
		}
		
		if(frame == 0){
		vacuumsprite.setTextureRect(IntRect(470,179,12,17));
		//check the direction of the player and then adjust the position of the vacuum accordingly
			if (speed < 0){
				vac_x = player_x-27;
				vac_y = player_y + 55;
				vacuumsprite.setPosition(vac_x,vac_y);
			}
			if(speed > 0){
				vac_x = player_x+27;
				vac_y = player_y + 55;
				vacuumsprite.setPosition(vac_x,vac_y);
			}
		}
		else if (frame == 1 ){
			vacuumsprite.setTextureRect(IntRect(440,177,24,20));
			if (speed < 0){
				vac_x = player_x-62;
				vac_y = player_y + 53;
				vacuumsprite.setPosition(vac_x,vac_y);
			}
			
			if (speed > 0){
				vac_x = player_x+62;
				vac_y = player_y + 55;
				vacuumsprite.setPosition(vac_x,vac_y);
			}
				
		}
		else if (frame == 2){
			vacuumsprite.setTextureRect(IntRect(400,176,31,24));
			if (speed<0){
				vac_x = player_x-83;
				vac_y = player_y + 51;
				vacuumsprite.setPosition(vac_x,vac_y);
			}
				
			if (speed > 0){
				vac_x = player_x+81;
				vac_y = player_y + 51;
				vacuumsprite.setPosition(vac_x,vac_y);
			}

		}
		
		if(speed > 0 ){
			vacuumsprite.setScale(-3,3);
		}
		if(speed < 0){
			vacuumsprite.setScale(3,3);
		}
		//make the vacuum usable in 4 directions using WASD
		if (Keyboard::isKeyPressed(Keyboard::D)){
			if (speed < 0){
			vacuumsprite.setScale(-3,3);

			vac_x = player_x+83+120;
			vac_y = player_y + 51;
			vacuumsprite.setPosition(vac_x,vac_y);

			}

		}

		if(Keyboard::isKeyPressed(Keyboard::A)){
			if(speed > 0){
			vacuumsprite.setScale(3,3);
			vac_x = player_x-83-120;
			vac_y = player_y + 55;
			vacuumsprite.setPosition(vac_x,vac_y);
			}
		}
			if (Keyboard::isKeyPressed(Keyboard::W)){
				vacupsprite.setScale(1,1);
				vac_x = speed > 0 ?player_x-96:player_x;
				vac_y = player_y - 96;//add player height
				
				vacupsprite.setPosition(vac_x,vac_y);
				
	}

			if (Keyboard::isKeyPressed(Keyboard::S)){
				vacupsprite.setScale(1,-1);
				vac_x = speed > 0 ?player_x-96:player_x;//adjust the postition of the vaccum according to the visuals of the player
				vac_y = player_y + 96 +120;
				vacupsprite.setPosition(vac_x,vac_y);
				
	}
		
		
		frame++;//goto next frame
		
	}}

	


}

void level_one(char **lvl,int width,int height,float ghost_x[8],float ghost_y[8],int ghost_speed[8],float skeleton_x[4],float skeleton_y[4],int skeleton_speed[4],float player_x,float player_y,int &lives,const int cell_size,int pwidth,int pheight,float &speed, Sprite ghostsprite[],bool isghostfacingleft[],int ghost_state[],int ghost_timer[],Sprite skeletonSprite[],bool isskeletonfacngleft[],int skeleton_state[],int skeleton_timer[], float& vac_x,float& vac_y,int& vacwidth,int& vacheight,bool isghostalive[],bool isskeletonalive[]){
	//platform to spawn player	
	for(int i=8 ; i < 10; i++){
		lvl[6][i] = '#';
	}

	for(int i=3;i<width-3;i++){//upper horizontal
		lvl[3][i] = '#';
	}
	for(int i=3;i<width-3;i++){//lower horizontal on vertical stand
		lvl[9][i] = '#';
	}
	//left platforms
	for(int i=0;i<5;i++){//upper left
		lvl[6][i] = '#';
	}
	//right platforms
	for(int i=18;i>width-6;i--){//upper right
		lvl[6][i] = '#';
	}
	
	ghosts(ghost_x,ghost_speed,8,ghostsprite,isghostfacingleft,ghost_state,ghost_timer);
	skeletons(skeleton_x,skeleton_y,skeleton_speed,4,skeletonSprite,isskeletonfacngleft,skeleton_state,skeleton_timer,cell_size);
	
	//lowest platform
	for(int i=0;i<width;i++){
		lvl[13][i] = '#';
	}
	for(int i = 0; i < 8; i++){
		if(onplatform(lvl,96,120,ghost_x[i],ghost_y[i],cell_size,ghost_speed[i]) == false){
			ghost_speed[i] *= -1;
		}
	}
	for (int i = 0; i < 8; i++){
		if(checkcollision(player_x,player_y,pwidth,pheight,ghost_x[i],ghost_y[i],60,80,speed,ghost_speed[i]) && isghostalive[i])//smaller dimensions of ghost passed to fix collision when jumping
		lives = 0;
	}
	
	for(int i = 0; i < 4; i++){
		if(onplatform(lvl,120,225,skeleton_x[i],skeleton_y[i],cell_size,skeleton_speed[i]) == false){
			skeleton_speed[i] *= -1;
		}
	}
	for (int i = 0; i < 4; i++){
		if(checkcollision(player_x,player_y,pwidth,pheight,skeleton_x[i],skeleton_y[i],60,175,speed,skeleton_speed[i]) && isskeletonalive[i])//smaller dimensions of skeleton passed to fix collision when jumping
		lives = 0;
	}
	
	if (Keyboard :: isKeyPressed(Keyboard::Space)){
		for (int i = 0; i < 8; i++){
			if(checkcollision(vac_x,vac_y,vacwidth,vacheight,ghost_x[i],ghost_y[i],120,120,speed,ghost_speed[i])){
				
				suck(speed,ghost_x[i],ghost_y[i],120,120,ghost_speed[i],player_x,player_y,pwidth,pheight,ghostsprite[i],isghostalive[i]);
				
			}
			
		}
	}
	if (Keyboard :: isKeyPressed(Keyboard::Space)){
		for (int i = 0; i< 4; i++){
		if (checkcollision(vac_x,vac_y,vacwidth,vacheight,skeleton_x[i],skeleton_y[i],60,175,speed,skeleton_speed[i])){
				suck(speed,skeleton_x[i],skeleton_y[i],60,175,skeleton_speed[i],player_x,player_y,pwidth,pheight,skeletonSprite[i],isskeletonalive[i]);
				
			}
		}
	}

	for (int i = 0;i < 8; i++){
		if(isghostalive[i])
			ghostsprite[i].setPosition(ghost_x[i],ghost_y[i]);
		}
	for (int i = 0; i < 4; i++){
		if (isskeletonalive[i]){
			skeletonSprite[i].setPosition(skeleton_x[i],skeleton_y[i]); 
		}
		
	}
	
	
}

bool check_level_completion(bool isghostalive[],bool isskeletonalive[],int current_level){
	int ghost_num,skeleton_num;
	if(current_level==1){
		ghost_num = 8;
		skeleton_num = 4;
	}
	// Check Ghosts
    for (int i = 0; i < ghost_num; i++) {
        if (isghostalive[i]) return false; 
    }
    // Check Skeletons
    for (int i = 0; i < skeleton_num; i++) { 
        if (isskeletonalive[i]) return false; 
    }
    return true; // Everyone is dead
}

void initialize_level2(char** lvl,int width,int height){
	//clear map
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++)
			lvl[i][j] = '\0';
	}

	//lowest platform
	for(int i=0;i<width;i++){
		lvl[13][i] = '#';
	}

	for(int i=4;i<12;i++){
		for(int j=4;j<11;j++)
			if(i==j)
				lvl[j][i]='#';
	}

}



int main()
{
	int lives = 1;
	int score = 0;
	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	//level specifics
	const int cell_size = 64;
	const int height = 14;
	const int width = 18;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;
	Texture chelnovtex;
	Sprite chelnov;
	Texture ghosttex;
	Sprite ghostsprite[8];
	Texture vacuumtex;
	Sprite vacuumsprite;
	Texture skeletonTex;
	Sprite skeletonSprite[4];
	Texture vacuptex;
	Sprite vacupsprite;



	ghosttex.loadFromFile("Assets/ghost.png");
	for (int i = 0; i < 8; i++){
		ghostsprite[i].setTexture(ghosttex);
		ghostsprite[i].setTextureRect(IntRect(0,0,40,40));
	}
	
	skeletonTex.loadFromFile("Assets/skeleton.png");
	for (int i = 0; i < 4; i++){
		skeletonSprite[i].setTexture(skeletonTex);
		skeletonSprite[i].setTextureRect(IntRect(0,0,40,75));
		
	}

	chelnovtex.loadFromFile("Assets/chelnov.png");
	chelnov.setTexture(chelnovtex);

	bgTex.loadFromFile("Data/bg1.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);

	vacuumtex.loadFromFile("Assets/tumblepoppers.png");
	vacuumsprite.setTexture(vacuumtex);
	vacuumsprite.setScale(3,3);
	vacuumsprite.setTextureRect(IntRect(470,179,12,17));
	vacuptex.loadFromFile("Assets/vacup.png");
	vacupsprite.setTexture(vacuptex);
	
	//Music initialisation
	Music lvlMusic;

	// lvlMusic.openFromFile("Data/mus.ogg");
	// lvlMusic.setVolume(20);
	// lvlMusic.play();
	// lvlMusic.setLoop(true);

	//random time every time game is opened
	srand(time(0));

	//player data
	float player_x = 650;
	float player_y = 150;
	float speed = 5;


	float ghost_x[8]={4*cell_size,15*cell_size,30,1000,4*cell_size,15*cell_size,30,15*cell_size};
	float ghost_y[8]={3*cell_size-120,3*cell_size-120,6*cell_size-120,6*cell_size-120,9*cell_size-120,9*cell_size-120,13*cell_size-120,13*cell_size-120};
	int ghost_state[8]={1,1,1,1,1,1,1,1}; //1 means moving
	int ghost_timer[8];
	for(int i = 0; i < 8; i++)
		ghost_timer[i] = rand() % (120);
	int ghost_speed[8] = {2,2,2,2,2,2,2,2};

	float skeleton_x[4]={5*cell_size,5*cell_size,4*cell_size,12*cell_size};
	float skeleton_y[4]={3*cell_size-225,9*cell_size-225,13*cell_size-225,13*cell_size-225};
	int skeleton_state[4]={1,1,1,1};
	int skeleton_timer[4];
	for(int i=0; i< 4; i++)
		skeleton_timer[i] = rand () % 120;
	int skeleton_speed[4]={3,3,3,3};

	float vac_x;
	float vac_y;

	const float jumpStrength = -200; // Initial jump velocity
	const float gravity = 1;  // Gravity acceleration

	bool isJumping = false;  // Track if jumping

	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;

	Texture PlayerTexture;
	Sprite PlayerSprite;

	bool onGround = false;
	float offset_x = 0;
	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	int PlayerHeight = 135;
	int PlayerWidth = 96;
	int vacwidth = 93;
	int vacheight = 72;

	bool up_button = false;

	char top_left = '\0';
	char top_right = '\0';
	char top_mid = '\0';

	char left_mid = '\0';
	char right_mid = '\0';

	char bottom_left = '\0';
	char bottom_right = '\0';
	char bottom_mid = '\0';

	char bottom_left_down = '\0';
	char bottom_right_down = '\0';
	char bottom_mid_down = '\0';

	char top_right_up = '\0';
	char top_mid_up = '\0';
	char top_left_up = '\0';

	PlayerTexture.loadFromFile("Assets/tumblepoppers.png");
	PlayerSprite.setTexture(PlayerTexture);
	PlayerSprite.setScale(-3,3);
	PlayerSprite.setPosition(player_x, player_y);
	PlayerSprite.setTextureRect(IntRect(12,36,32,45));

	
	for (int i = 0; i< 8; i++){
		ghostsprite[i].setScale(3,3);
	}

	for (int i = 0; i< 4; i++){
		skeletonSprite[i].setScale(3,3);
	}
	

	//creating level array
	lvl = new char* [height];
	for (int i = 0; i < height; i += 1)
	{
		lvl[i] = new char[width];
	}

	RectangleShape hitbox;
	hitbox.setSize(Vector2f(96,96));
	float hitx = player_x;


	int frame = 0;
	int timer = 0;
	int vacframe = 0;
	int vactim = 0;
	bool isdead = false;
	bool isfacingleft = false;
	bool isskeletonfacingleft[8];
	bool isghostalive[8];
	bool isskeletonalive[4];
	for (int i = 0; i<8; i++){
		
		isskeletonfacingleft[i] = false;
		isskeletonalive[i] = true;
	}

	bool isghostfacingleft[8];
	for (int i = 0; i<8; i++){
		
		isghostalive[i] = true;
		isghostfacingleft[i] = false;
	}

	
	//levels
	int current_level=1;
	int level2Loaded = false;


	Event ev;
	//main loop
	while (window.isOpen())
	{

		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed) 
			{
				window.close();
			}

			if (ev.type == Event::KeyPressed)
			{
			}
			if (Keyboard :: isKeyPressed(Keyboard::Up) && onGround){
				jump(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
				onGround = false;
			}
		}
		if (speed < 0)
		isfacingleft = true;
		//presing escape to close
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}
		window.clear();

		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth,isfacingleft);
		PlayerSprite.setPosition(player_x, player_y);
		if (lives == 0){
			playerdies(PlayerSprite,frame,timer);
			isdead = true;
		}
		else{
		if (Keyboard::isKeyPressed(Keyboard::Right) && !isdead)
		{	
			if (isfacingleft){
			player_x+=96;
			isfacingleft = false;
			}	
			PlayerSprite.setPosition(player_x, player_y);
			PlayerSprite.setScale(-3,3);

			moveright(player_x,speed,PlayerSprite,frame,timer);
			
		}
		else if (Keyboard::isKeyPressed(Keyboard::Left) && !isdead){
			if (isfacingleft == false){
				player_x -= 96;
				isfacingleft = true;
			}
			PlayerSprite.setPosition(player_x, player_y);
			moveleft(player_x,speed,PlayerSprite,frame,timer);
		}
		
	
		else if (!onGround){//jumping animation
			PlayerSprite.setTextureRect(IntRect(525,30,30,42));
		}
		else {//stand still animation
		PlayerSprite.setTextureRect(IntRect(12,36,32,45));
		frame = 0;
		}
		//vaccum
		//vaccum
		if (Keyboard::isKeyPressed(Keyboard::Space)){
			getvacuum(vacuumsprite,vacupsprite,player_x,player_y,vacframe,vactim,speed,vac_x,vac_y);
			if(Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Space) && !(Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::S)) )
			window.draw(vacuumsprite);
			if(Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::S))
			window.draw(vacupsprite);
	
		

	}
	}

		if (PlayerSprite.getScale().x < 0){
			hitx = player_x - 100;
		}
		else if (PlayerSprite.getScale().x >0){
			hitx = player_x;
		}
		hitbox.setPosition(hitx,player_y);
		hitbox.setFillColor(Color::Transparent);
		hitbox.setOutlineColor(Color::Red);
		hitbox.setOutlineThickness(2);
		window.draw(hitbox);

		if(!(isdead== true && frame > 5))
			window.draw(PlayerSprite);


		if(current_level==1){
		level_one(lvl,width,height,ghost_x,ghost_y,ghost_speed,skeleton_x,skeleton_y,skeleton_speed,player_x,player_y,lives,cell_size,PlayerWidth,PlayerHeight,speed,ghostsprite,isghostfacingleft,ghost_state,ghost_timer,skeletonSprite,isskeletonfacingleft,skeleton_state,skeleton_timer,vac_x,vac_y,vacwidth,vacheight,isghostalive,isskeletonalive);
		
		
		for (int i = 0;i < 8; i++){
			ghostsprite[i].setPosition(ghost_x[i],ghost_y[i]);
		}
		for (int i = 0;i < 4; i++){
			skeletonSprite[i].setPosition(skeleton_x[i],skeleton_y[i]);
		}

		

		float current_speed_x = 0;
		
		

		platform_collision_y(lvl,offset_x,current_speed_x,player_x,player_y,cell_size,PlayerHeight,PlayerWidth);
		player_x += current_speed_x;

		for(int i = 0; i < 8; i++){
			if(isghostalive[i])
				window.draw(ghostsprite[i]);}
		
		for(int i = 0; i < 4; i++){
			if(isskeletonalive[i])
				window.draw(skeletonSprite[i]);}
	
	

	if(check_level_completion(isghostalive,isskeletonalive,current_level))
			current_level = 2;
	
}
else if(current_level==2){
	if(!level2Loaded){
		initialize_level2(lvl,width,height);
		level2Loaded = true;


	}





}
	int ghosts_left = 0;
    int skels_left = 0;
    for(int i=0; i<8; i++) if(isghostalive[i]) ghosts_left++;
    for(int i=0; i<4; i++) if(isskeletonalive[i]) skels_left++;
    
    cout << "Ghosts: " << ghosts_left << " | Skeletons: " << skels_left << endl;
	window.display();


}
	
	//stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	return 0;
}




