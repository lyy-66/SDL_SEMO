#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <SDL_mixer.h> // 音乐

// 游戏对象结构体
struct GameObject {
    SDL_Texture* texture;
    SDL_Rect position;
    int speedX;
    int speedY;
    bool isActive;
    SDL_Rect collider;
};

// 修改场景状态枚举
enum GameState {
    MENU,
    TRANSITION,  // 新增过渡状态
    OPTIONS,     // 新增选项状态
    GAME,
    INSTRUCTION, // 玩法介绍
    SETTINGS,    // 设置
    WIN,
    CUTSCENE,
    EXIT_SCREEN,
    EXIT,       // 退出
};
//定义音乐类型枚举
enum MusicType {
    MENU_MUSIC,//菜单界面
    GAME_MUSIC,//打斗
    JUQING_MUSIC,//剧情音乐
    FEIDADOU_MUSIC,//非打斗音乐
    SHENGLI_MUSIC,//胜利音乐
    NO_MUSIC//
};
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);  // 初始化SDL_image
    // 添加时间记录变量（在main函数开头）
    // 初始化SDL和SDL_mixer
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    Uint32 transitionStartTime = 0;
    bool transitionCompleted = false;
    std::vector<SDL_Texture*> cutsceneTextures;  // 过场动画纹理集合
    int currentCutsceneIndex = 0;                // 当前播放的图片索引
    Uint32 cutsceneStartTime = 0;                // 过场动画开始时间

    // 定义选项按钮区域（根据实际图片调整）
    SDL_Rect optionRects[4] = {
        {500, 100, 200, 50},  // 游戏地图
        {500, 200, 200, 50},  // 玩法介绍
        {500, 300, 200, 50},  // 设置
        {500, 400, 200, 50}   // 退出
    };

    // 创建窗口和渲染器
    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 600;
    const int GROUND_Y = 470; //原点在左上角
    SDL_Window* window = SDL_CreateWindow(
        "双狼游戏",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // ========== 加载所有资源 ========== //
    const char* menuImagePath = "1.bmp";
    const char* gameMapPath = "2.bmp";
    const char* playerPath = u8"主人公.bmp";
    const char* obstaclePath = u8"障碍物.bmp";
    const char* monsterPath = u8"原石虫.bmp";
    const char* doorPath = u8"门.bmp";
    const char* transitionPath = u8"陷下去.png";//陷下去
    const char* optionsPath = u8"四个选项.png";//选项
    const char* instructionPath = u8"HELP.png";//HELP
    const char* settingsPath = u8"SETTINGS.png";//SETTING
    const char* exitImagePath = u8"退出画面.png";
    const char* gameMap2Texture = u8"地图2.png";
    const char* cutscenePaths[] = {
    u8"1.png",
    u8"2.png",
    u8"3.png",
    u8"4.png",
    u8"5.png",
    u8"6.png",
    u8"7.png",
    u8"8.png",
    u8"9.png",
    u8"10.png",
    u8"11.png",
    u8"12.png",
    u8"13.png",
    u8"14.png",
    u8"15.png",
    u8"16.png",
    u8"17.png",
    u8"18.png",
    u8"19.png"
    };
    // 音乐
    musicManager.Load(MENU_MUSIC, u8"menu.wav");//菜单背景音乐
    musicManager.Load(GAME_MUSIC, u8"game.wav");//打斗音乐
    musicManager.Load(JUQING_MUSIC, u8"剧情.wav");//剧情音乐
    musicManager.Load(SHENGLI_MUSIC, u8"游戏胜利.wav");//游戏胜利
    musicManager.Load(FEIDADOU_MUSIC, u8"feidadou.wav");//非打斗音乐
    musicManager.Load(NO_MUSIC, u8"no.wav");
    SDL_Texture* exitTexture = IMG_LoadTexture(renderer, exitImagePath);

    // 加载纹理并检查
    SDL_Texture* menuTexture = IMG_LoadTexture(renderer, menuImagePath);
    SDL_Texture* gameMapTexture = IMG_LoadTexture(renderer, gameMapPath);
    // 加载并启用透明通道
    SDL_Texture* playerTexture = IMG_LoadTexture(renderer, playerPath);
    SDL_SetTextureBlendMode(playerTexture, SDL_BLENDMODE_BLEND);
    SDL_Texture* obstacleTexture = IMG_LoadTexture(renderer, obstaclePath);
    SDL_Texture* monsterTexture = IMG_LoadTexture(renderer, monsterPath);
    SDL_Texture* doorTexture = IMG_LoadTexture(renderer, doorPath);
    // 在main函数开头添加纹理声明
    SDL_Texture* transitionTexture = nullptr;
    SDL_Texture* optionsTexture = nullptr;
    SDL_Texture* instructionTexture = nullptr;
    SDL_Texture* settingsTexture = nullptr;
    // 加载新增纹理
    transitionTexture = IMG_LoadTexture(renderer, transitionPath);
    optionsTexture = IMG_LoadTexture(renderer, optionsPath);
    instructionTexture = IMG_LoadTexture(renderer, instructionPath);
    settingsTexture = IMG_LoadTexture(renderer, settingsPath);

    // 错误检查
    if (!menuTexture || !gameMapTexture || !playerTexture || !obstacleTexture || !monsterTexture || !doorTexture) {
        SDL_Log("资源加载失败: %s", IMG_GetError());
        return -1;
    }
    // 在资源加载后添加检查
    if (!transitionTexture || !optionsTexture || !instructionTexture || !settingsTexture) {
        SDL_Log("新增资源加载失败: %s", IMG_GetError());
        return -1;
    }

    // 添加错误检查
    if (!exitTexture) {
        SDL_Log("退出画面加载失败: %s", IMG_GetError());
        return -1;
    }
    // 加载过场动画纹理
    for (const auto& path : cutscenePaths) {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path);
        if (!tex) {
            SDL_Log("过场动画资源加载失败: %s", IMG_GetError());
            return -1;
        }
        cutsceneTextures.push_back(tex);
    }

    // ========== 初始化游戏对象 ========== //
    GameObject player = {
        playerTexture,
       {100, GROUND_Y - 50 , 50, 50}, // 初始位置在地面上（地面 Y=470，角色高 80）
        0,
        0,
        false,
        {110, GROUND_Y - 45, 30, 45}
    };

    GameObject door = {
        doorTexture,
        {3000, GROUND_Y - 100, 80, 100}, // 门的位置
        0,
        0,
        false,
            {3000, GROUND_Y - 100, 80, 100}
    };

    std::vector<SDL_Rect> obstacles = {
        {500, GROUND_Y - 50, 50, 50},// 障碍物位置
        {550, GROUND_Y - 50, 50, 50},
        {600, GROUND_Y - 50, 50, 50},
        {650, GROUND_Y - 50, 50, 50},
        {550, GROUND_Y - 100, 50, 50},
        {600, GROUND_Y - 100, 50, 50},
        //第二个三角锥
         {800, GROUND_Y - 50, 50, 50},
         {850, GROUND_Y - 50, 50, 50},
         {900, GROUND_Y - 50, 50, 50},
         {850, GROUND_Y - 100, 50, 50},

         {1200, GROUND_Y - 50, 50, 50},
         {1250, GROUND_Y - 100, 50, 50},
         {1300, GROUND_Y - 100, 50, 50},
         {1250, GROUND_Y - 50, 50, 50},
         {1300, GROUND_Y - 50, 50, 50},
         //比较高的平面

         {1400, GROUND_Y - 100, 50, 50},
         {1450, GROUND_Y - 100, 50, 50},
         {1500, GROUND_Y - 100, 50, 50},
         {1550, GROUND_Y - 100, 50, 50},
         {1600, GROUND_Y - 100, 50, 50},

         {1700, GROUND_Y - 100, 50, 50},
         {1750, GROUND_Y - 100, 50, 50},
         {1800, GROUND_Y - 100, 50, 50},
         {1800, GROUND_Y - 150, 50, 50},
    };

    std::vector<GameObject> monsters = {
        {monsterTexture, {550, GROUND_Y - 40, 40, 40}, 2, 0, false},
        {monsterTexture, {850, GROUND_Y - 140, 40, 40}, -2, 0, false}
    };

    SDL_Rect camera = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // 摄像机初始化
    // ========== 游戏主循环 ========== //
    GameState currentState = MENU;
    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        // 事件处理
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            // 菜单点击处理
            if (currentState == MENU && event.type == SDL_MOUSEBUTTONDOWN) {//处于菜单界面而且检测到鼠标点击
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY); // 获取鼠标坐标
                SDL_Rect startButton = { 500, 300, 200, 100 };//start这个键的坐标，x，y，宽度，高度
                SDL_Point mousePos = { mouseX, mouseY }; // 创建 SDL_Point 实例
                if (SDL_PointInRect(&mousePos, &startButton)) { // 检测鼠标是否在start按钮区域内
                    currentState = TRANSITION;
                    transitionStartTime = SDL_GetTicks(); // 记录过渡开始时间
                }
            }

            // 选项界面点击处理
            if (currentState == OPTIONS && event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                SDL_Point mousePos = { mouseX, mouseY };

                for (int i = 0; i < 4; ++i) {
                    if (SDL_PointInRect(&mousePos, &optionRects[i])) {
                        switch (i) {
                        case 0: currentState = GAME; break;    // 游戏地图
                        case 1: currentState = INSTRUCTION; break;//HELP
                        case 2: currentState = SETTINGS; break;//SETTINGS
                            // 修改选项点击处理
                        case 3:
                            currentState = EXIT_SCREEN;  // 原"退出游戏"改为进入退出画面
                            transitionStartTime = SDL_GetTicks(); // 记录退出画面开始时间
                            break;

                        }
                        break;
                    }
                }
            }
            // 状态更新
            switch (currentState)
            {
            case TRANSITION:
                // 过渡画面显示0.5秒
                if (SDL_GetTicks() - transitionStartTime > 500) {
                    currentState = OPTIONS;
                }
                break;
            case EXIT_SCREEN:
                // 显示1秒后退出
                if (SDL_GetTicks() - transitionStartTime > 1000) {
                    isRunning = false;
                }
                break;
                // 在状态更新部分添加（主循环内）
            case CUTSCENE: {
                Uint32 elapsed = SDL_GetTicks() - cutsceneStartTime;
                // 每2000ms切换一张图片
                if (elapsed > 2000) {
                    currentCutsceneIndex++;
                    cutsceneStartTime = SDL_GetTicks();

                    // 所有图片播放完毕
                    if (currentCutsceneIndex >= cutsceneTextures.size()) {
                        // 切换到第二张地图（假设已加载gameMap2Texture）
                        gameMapTexture = IMG_LoadTexture(renderer, "D:\\SDL_Project\\SDL_Demo\\SDL_Demo\\地图2.png");
                        currentState = GAME;

                        // 重置玩家位置到新地图起点
                        player.position = { 100, GROUND_Y, 50, 50 };
                        camera.x = 0;
                    }
                }
                break;
            }
            }
            // 音乐
            switch (currentState) {
            case MENU:
                musicManager.Play(MENU_MUSIC);//菜单音乐
                break;
            case TRANSITION:
                musicManager.Play(MENU_MUSIC, 0); // 不循环
                break;
            case OPTIONS:
                musicManager.Play(MENU_MUSIC);//菜单音乐
                break;
            case GAME:
                musicManager.Play(FEIDADOU_MUSIC);//非打斗
                break;
            case WIN:
                musicManager.Play(SHENGLI_MUSIC);//胜利
                break;
            case CUTSCENE:
                musicManager.Play(JUQING_MUSIC);//剧情
                break;
            default:
                musicManager.Stop();
            }

            // ========== 游戏逻辑更新 ========== //
            if (currentState == GAME) {

                //保存旧坐标
                int oldX = player.position.x;
                int oldY = player.position.y;

                // 角色控制
                const Uint8* keystates = SDL_GetKeyboardState(NULL);
                player.speedX = (keystates[SDL_SCANCODE_LEFT] ? -5 : 0) + (keystates[SDL_SCANCODE_RIGHT] ? 5 : 0);
                player.position.x += player.speedX;
                player.collider.x = player.position.x + 10;

                // 水平碰撞检测
                for (auto& obs : obstacles) {
                    if (SDL_HasIntersection(&player.collider, &obs)) {
                        player.position.x = oldX;
                        player.collider.x = oldX + 10; // 这里不加偏移量
                        break;
                    }
                }
                if (keystates[SDL_SCANCODE_SPACE] && !player.isActive) {
                    player.speedY = -15;
                    player.isActive = true;
                }
                player.speedY += 1;
                player.position.y += player.speedY;
                player.collider.y = player.position.y + 5;

                // 垂直碰撞检测
                bool onGround = false;
                for (auto& obs : obstacles) {
                    if (SDL_HasIntersection(&player.collider, &obs)) {
                        if (player.speedY > 0) { // Falling down
                            player.position.y = obs.y - player.position.h;
                            player.speedY = 0;
                            onGround = true;
                        }
                        else if (player.speedY < 0) { // Jumping up
                            player.position.y = obs.y + obs.h;
                            player.speedY = 0;
                        }
                        player.collider.y = player.position.y + 5;
                        break;
                    }
                }

                if (!onGround && player.position.y >= GROUND_Y - 50) {
                    player.position.y = GROUND_Y - 50;
                    player.speedY = 0;
                    onGround = true;
                }

                player.isActive = !onGround; // Update isActive based on whether the player is on the ground

                for (auto& monster : monsters) {
                    monster.position.x += monster.speedX;
                    for (auto& obs : obstacles) {
                        if (SDL_HasIntersection(&monster.position, &obs)) {
                            monster.speedX *= -1;
                            monster.position.x += (monster.speedX > 0) ? 2 : -2;
                            break;
                        }
                    }
                }
                // 摄像机控制
                camera.x = player.position.x - WINDOW_WIDTH / 2;
                camera.x = SDL_clamp(camera.x, 0, 3000 - WINDOW_WIDTH);

                // 胜利检测
                if (SDL_HasIntersection(&player.position, &door.position)) {
                    currentState = CUTSCENE;
                    currentCutsceneIndex = 0;
                    cutsceneStartTime = SDL_GetTicks();  // 记录过场动画开始时间
                }
            }

        }

        // 渲染部分
        SDL_RenderClear(renderer);
        switch (currentState) {
            //渲染菜单
        case MENU: {
            SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
            break;
        }
        case TRANSITION: {
            SDL_RenderCopy(renderer, transitionTexture, NULL, NULL);
            break;
        }
        case OPTIONS: {
            SDL_RenderCopy(renderer, optionsTexture, NULL, NULL);
            break;
        }
        case INSTRUCTION: {
            SDL_RenderCopy(renderer, instructionTexture, NULL, NULL);
            break;
        }
                        // 在渲染部分添加case
        case CUTSCENE: {
            if (currentCutsceneIndex < cutsceneTextures.size()) {
                SDL_RenderCopy(renderer, cutsceneTextures[currentCutsceneIndex], NULL, NULL);
            }
            break;
        }
        case GAME: {
            // 渲染地图
            SDL_RenderCopy(renderer, gameMapTexture, NULL, NULL);

            // 渲染障碍物
            for (auto& obs : obstacles) {
                SDL_Rect renderPos = { obs.x - camera.x, obs.y, obs.w, obs.h };
                SDL_RenderCopy(renderer, obstacleTexture, NULL, &renderPos);
            }

            // 渲染角色
            SDL_Rect playerPos = { player.position.x - camera.x, player.position.y, 50, 50 };
            SDL_RenderCopy(renderer, playerTexture, NULL, &playerPos);

            // 渲染怪物
            for (auto& monster : monsters) {
                SDL_Rect monsterPos = { monster.position.x - camera.x, monster.position.y, 40, 40 };
                SDL_RenderCopy(renderer, monsterTexture, NULL, &monsterPos);
            }

            // 渲染门
            SDL_Rect doorPos = { door.position.x - camera.x, door.position.y, 80, 100 };
            SDL_RenderCopy(renderer, doorTexture, NULL, &doorPos);
            break;

        }
                 //渲染胜利部分
        case WIN: {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderClear(renderer);
            break;
        }

        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    // ========== 资源清理 ========== //
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(gameMapTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(obstacleTexture);
    SDL_DestroyTexture(monsterTexture);
    SDL_DestroyTexture(doorTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    for (auto& tex : cutsceneTextures) {
        SDL_DestroyTexture(tex);
    }
    SDL_DestroyTexture(transitionTexture);
    SDL_DestroyTexture(optionsTexture);
    SDL_DestroyTexture(instructionTexture);
    SDL_DestroyTexture(settingsTexture);
    SDL_DestroyTexture(exitTexture);
    return 0;
}