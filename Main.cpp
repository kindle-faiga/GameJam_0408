# include <Siv3D.hpp>

struct Effect
{
public:
    Effect(Vec2 pos):m_pos(pos)
    {
        m_count = System::FrameCount();
    }
    
    void draw()
    {
        if(120 < System::FrameCount()-m_count)
        {
            m_pos = Vec2(10000,10000);
        }
        
        for(auto x : step(10))
        {
            //Circle(m_pos + Circular(170, Radians(x * 30)), x % 3 ? 10 : 15).draw();
            Circle(m_pos + Circular((System::FrameCount()-m_count)*6, (x * 36)*(3.14/180)), 15).draw(HSV(x*30,0.8));
        }
    }
    Vec2 m_pos;
private:
    int m_count;
};

struct Cube
{
public:
    Cube(int size, Vec2 pos)
    :m_size(size)
    ,m_pos(pos)
    ,m_speed(3.0)
    ,m_range(20)
    ,m_titRange(0)
    ,m_state(0)
    ,m_alive(true)
    ,m_move(true)
    ,m_set(false)
    {
        m_color = Random(0,7);
        m_count = System::FrameCount();
    }
    
    Circle getTopPos(){ return Circle(m_pos.x+m_size/2+m_range,m_pos.y+m_titRange,10);}
    Circle getleftPos(){ return Circle(m_pos.x+m_range,m_pos.y+m_size/2+m_titRange,30);}
    Circle getrightPos(){ return Circle(m_pos.x+m_size,m_pos.y+m_size/2+m_titRange,30);}
    Circle getdownPos(){ return Circle(m_pos.x+m_size/2,m_pos.y+m_size,10);}
    
    bool getMove(){return m_move;}
    
    void setMove(bool move)
    {
        m_move = move;
    }
    
    bool getSet(){return m_set;}
    void setSet(){m_set = true;}
    
    bool getAlive(){return m_alive;}
    
    void dead(){m_alive = false;}
    
    int getColor(){return m_color;}
    
    int getState(){return m_state;}
    
    Vec2 getPos(){return m_pos;}
    
    void setPos(Vec2 pos){m_pos = pos;}
    
    void setState(int state)
    {
        m_state = state;
    }
    
    void update()
    {
        if((Window::Size().y-m_size*2) <= m_pos.y)
        {
            m_pos.y = (Window::Size().y-m_size*2);
            m_move = false;
            m_set = true;
        }
        
        if(m_move)
        {
            m_pos.moveBy(0, m_speed);
            m_count = System::FrameCount();
        }
        else
        {
            /*
            if(m_state == 0)
            {
                if((System::FrameCount()-m_count) > 600)
                {
                    m_titRange = m_size/2;
                }
            }
            else if
            */
            if(m_state == 1)
            {
                if((System::FrameCount()-m_count) > 240)
                {
                    m_titRange = m_size/2;
                }
            }
        }
    }
    
    void draw()const
    {
        switch(m_state)
        {
            case 0:
                RoundRect(m_pos.x, m_pos.y+m_titRange, m_size, m_size-m_titRange, 10).draw();
                break;
            case 1:
                RoundRect(m_pos.x, m_pos.y+m_titRange, m_size, m_size-m_titRange, 10).draw(Palette::Aqua);
                break;
            case 2:
                RoundRect(m_pos.x, m_pos.y+m_titRange, m_size, m_size-m_titRange, 10).draw(Palette::Gray);
                break;
        }
        
        RoundRect(m_pos.x+m_range/2, m_pos.y+m_range/2+m_titRange, m_size-m_range, m_size-m_range-m_titRange, 10).draw(HSV(m_color*40));
        
        /*
        Circle(m_pos.x+m_size/2+m_range,m_pos.y+m_titRange,10).draw();
        Circle(m_pos.x+m_range,m_pos.y+m_size/2+m_titRange,30).draw();
        Circle(m_pos.x+m_size,m_pos.y+m_size/2+m_titRange,30).draw();
        Circle(m_pos.x+m_size/2,m_pos.y+m_size,10).draw();
        */
    }
private:
    Vec2 m_pos;
    int m_size;
    int m_color;
    int m_range;
    int m_titRange;
    bool m_alive;
    bool m_move;
    double m_speed;
    int m_count;
    int m_state;
    bool m_set;
};

enum class State
{
    Rain,
    Drop,
    Thunder
};

void Main()
{
    Window::Resize(540, 720);
    Graphics::SetBackground(ColorF(0.2,0.5,0.3));
    
    State state = State::Rain;
    
    const Vec2 winCenter(Window::Center());
    const Vec2 winSize(Window::Size());
    const Point grid(6,8);
    
    Vec2 player(winCenter.x, winCenter.y/4);
    const Texture playerIcon(Icon(0x0f0c2,80));
    const Texture playerIconFrame(Icon(0x0f0c2,100));
    
    double rainSpeed = 5.0;
    const Texture rainIcon(Icon(0x0f043,40));
    const Texture rainIconFrame(Icon(0x0f043,50));
    const Texture dropIcon(Emoji(L"🍬"));
    const Texture thunderIcon(Icon(0x0f0e7,50));
    const Texture thunderIconFrame(Icon(0x0f0e7,60));
    Array<Vec2> rains;
    Array<Vec2> drops;
    Array<Vec2> thunders;
    int thunderCount = 0;
    
    const int size(90);
    const Point block(size,size);
    
    int frameCount = 0;
    int thunderNum = 0;
    int setCount = 180;
    Array<int> cubePos;
    Array<Cube> cubes;
    Array<Effect> effects;
    
    int score = 0;
    Font font(50,Typeface::Bold);
    
    for(auto x : step(grid.x))
    {
        cubePos.push_back(x*size);
    }
    
	while (System::Update())
	{
        if(Cursor::Pos().intersects(RectF(0,0,winSize.x, winSize.y)))
        {
             player.x = Cursor::Pos().x;
            
             if(Cursor::Pos().intersects(RectF(0,0,size, size)))
             {
                 state = State::Rain;
             }
             else if(Cursor::Pos().intersects(RectF(winSize.x-size,0,size, size)))
             {
                 state = State::Drop;
             }
             else if(Cursor::Pos().intersects(RectF(0,size,size, size)))
             {
                 if(510 < System::FrameCount()-thunderCount)
                 {
                     thunderCount = System::FrameCount();
                     state = State::Thunder;
                 }
             }
        }
        
        if(MouseL.down())
        {
            switch (state) {
                case State::Rain:
                    rains.emplace_back(player);
                    break;
                case State::Drop:
                    drops.emplace_back(player);
                    break;
                case State::Thunder:
                    if(thunderNum < 5)
                    {
                        ++thunderNum;
                        thunders.emplace_back(player);
                    }
                    break;
                default:
                    break;
            }
            
            if(thunderNum == 5)
            {
                thunderNum = 0;
                state = State::Drop;
            }
        }
        
        if((System::FrameCount()-frameCount) == setCount)
        {
            frameCount = System::FrameCount();
            int n = Random(1, grid.x-2);
            cubes.emplace_back(size, Vec2(cubePos[n],-size));
        }
        
        for(auto& rain : rains)
        {
            rain.moveBy(0, rainSpeed);
        }
        
        for(auto& drop : drops)
        {
            drop.moveBy(0, rainSpeed);
        }
        
        for(auto& thunder : thunders)
        {
            thunder.moveBy(0, rainSpeed);
        }
        
        for(auto x : step(cubes.size()))
        {
            bool change = false;
            
            for(auto y : step(cubes.size()))
            {
                if(x!=y && cubes[x].getdownPos().intersects(cubes[y].getTopPos()))
                {
                    change = true;
                    cubes[x].setSet();
                    cubes[x].setMove(false);
                    
                    if(cubes[x].getColor() == cubes[y].getColor() && cubes[x].getSet() && cubes[y].getSet())
                    {
                        if(cubes[x].getState()!=2)cubes[x].dead();
                        if(cubes[y].getState()!=2)cubes[y].dead();
                    }
                }
                
                if(x!=y && cubes[x].getTopPos().intersects(cubes[y].getdownPos()))
                {
                    if(cubes[x].getColor() == cubes[y].getColor() && cubes[x].getSet() && cubes[y].getSet())
                    {
                        if(cubes[x].getState()!=2)cubes[x].dead();
                        if(cubes[y].getState()!=2)cubes[y].dead();
                    }
                }
                
                if(x!=y && cubes[x].getrightPos().intersects(cubes[y].getleftPos()))
                {
                    if(cubes[x].getColor() == cubes[y].getColor() && cubes[x].getSet() && cubes[y].getSet())
                    {
                        if(cubes[x].getState()!=2)cubes[x].dead();
                        if(cubes[y].getState()!=2)cubes[y].dead();
                    }
                }
                
                if(x!=y && cubes[x].getleftPos().intersects(cubes[y].getrightPos()))
                {
                    if(cubes[x].getColor() == cubes[y].getColor() && cubes[x].getSet() && cubes[y].getSet())
                    {
                        if(cubes[x].getState()!=2)cubes[x].dead();
                        if(cubes[y].getState()!=2)cubes[y].dead();
                    }
                }
            }
            
            if(!change)
            {
                cubes[x].setMove(true);
            }
        }
        
        for(auto& cube : cubes)
        {
            for(auto& rain : rains)
            {
                if(Circle(rain,30).intersects(cube.getTopPos()) && cube.getState()!=2)
                {
                    rain.moveBy(0, 1000);
                    cube.setState(1);
                }
            }
            
            for(auto& drop : drops)
            {
                if(Circle(drop,30).intersects(cube.getTopPos()))
                {
                    drop.moveBy(0,1000);
                    cube.setState(2);
                }
            }
            
            for(auto& thunder : thunders)
            {
                if(Circle(thunder,50).intersects(cube.getTopPos()))
                {
                    if(cube.getState()==0)
                    {
                        thunder.moveBy(0,1000);
                    }
                    else if(cube.getState()==2)
                    {
                        cube.dead();
                        thunder.moveBy(0,1000);
                    }
                }
            }
            
            cube.update();
        }
        
        for(auto& cube : cubes)
        {
            if(!cube.getAlive())
            {
                score += 10;
                effects.emplace_back(cube.getPos());
                cube.setPos(Vec2(-1000,1000));
            }
        }
        
        Erase_if(rains, [](const Vec2& r) { return r.y > 1000; });
        Erase_if(drops, [](const Vec2& d) { return d.y > 1000; });
        Erase_if(thunders, [](const Vec2& t) { return t.y > 1000; });
        Erase_if(effects, [](const Effect& e) { return e.m_pos.y > 1000; });
        Erase_if(cubes, [](Cube& c) { return c.getPos().y > 1000; });
        
        RectF(0,winSize.y-size,winSize.x,size).draw(Palette::Gray);
        RectF(0,0,size,winSize.y).draw(Palette::Gray);
        RectF(winSize.x-size,0,size,winSize.y).draw(Palette::Gray);
        
        for(auto x : step(grid.x))
        {
            for(auto y : step(grid.y))
            {
                RectF(x*size, y*size, block).drawFrame(2.5,0,{255,255,255,127});
            }
        }
        
        rainIconFrame.drawAt(Vec2(size/2,size/2),Palette::Black);
        rainIcon.drawAt(Vec2(size/2,size/2), Palette::Aqua);
        
        dropIcon.scale(0.5).drawAt(Vec2(winSize.x-size/2,size/2));
        
        thunderIcon.scale(0.5).drawAt(Vec2(size/2,size*1.5),Alpha(System::FrameCount()-thunderCount));
        
        if(System::FrameCount()-thunderCount < 510)
        {
            thunderIconFrame.drawAt(Vec2(size/2,size*1.5),Alpha((System::FrameCount()-thunderCount)/2));
            thunderIcon.drawAt(Vec2(size/2,size*1.5),Alpha((System::FrameCount()-thunderCount)/2));
        }
        else
        {
            thunderIconFrame.drawAt(Vec2(size/2,size*1.5),Palette::Black);
            thunderIcon.drawAt(Vec2(size/2,size*1.5));
        }
        
        for(const auto& cube : cubes)
        {
            cube.draw();
        }
        
        playerIconFrame.drawAt(player,Palette::Black);
        switch(state)
        {
            case State::Rain:
                playerIcon.drawAt(player,Palette::Aqua);
                break;
            case State::Drop:
                playerIcon.drawAt(player,Palette::Gray);
                dropIcon.scale(0.3).drawAt(player);
                break;
            case State::Thunder:
                playerIcon.drawAt(player,Palette::Black);
                thunderIconFrame.drawAt(player,Palette::Black);
                thunderIcon.drawAt(player);
                break;
            default:
                break;
                
        }
        for(const auto& rain : rains)
        {
            rainIconFrame.drawAt(rain,Palette::Black);
            rainIcon.drawAt(rain, Palette::Aqua);
        }
        
        for(const auto& drop : drops)
        {
            dropIcon.scale(0.5).drawAt(drop);
        }
        
        for(const auto& thunder : thunders)
        {
            thunderIconFrame.drawAt(thunder,Palette::Black);
            thunderIcon.drawAt(thunder);
        }
        
        for(auto& effect: effects)
        {
            effect.draw();
        }
        
        
        //font(L"Score : " + score).drawAt(winCenter.x, winSize.y-50, Palette::Red);
	}
}
