#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <SDL_mixer.h> // ����

// ��Ϸ����ṹ��
struct GameObject {
    SDL_Texture* texture;
    SDL_Rect position;
    int speedX;
    int speedY;
    bool isActive;
    SDL_Rect collider;
};

// �޸ĳ���״̬ö��
enum GameState {
    MENU,
    TRANSITION,  // ��������״̬
    OPTIONS,     // ����ѡ��״̬
    GAME,
    INSTRUCTION, // �淨����
    SETTINGS,    // ����
    WIN,
    CUTSCENE,
    EXIT_SCREEN,
    EXIT,       // �˳�
};
//������������ö��
enum MusicType {
    MENU_MUSIC,//�˵�����
    GAME_MUSIC,//��
    JUQING_MUSIC,//��������
    FEIDADOU_MUSIC,//�Ǵ�����
    SHENGLI_MUSIC,//ʤ������
    NO_MUSIC//
};
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);  // ��ʼ��SDL_image
    // ���ʱ���¼��������main������ͷ��
    // ��ʼ��SDL��SDL_mixer
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    Uint32 transitionStartTime = 0;
    bool transitionCompleted = false;
    std::vector<SDL_Texture*> cutsceneTextures;  // ��������������
    int currentCutsceneIndex = 0;                // ��ǰ���ŵ�ͼƬ����
    Uint32 cutsceneStartTime = 0;                // ����������ʼʱ��

    // ����ѡ�ť���򣨸���ʵ��ͼƬ������
    SDL_Rect optionRects[4] = {
        {500, 100, 200, 50},  // ��Ϸ��ͼ
        {500, 200, 200, 50},  // �淨����
        {500, 300, 200, 50},  // ����
        {500, 400, 200, 50}   // �˳�
    };

    // �������ں���Ⱦ��
    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 600;
    const int GROUND_Y = 470; //ԭ�������Ͻ�
    SDL_Window* window = SDL_CreateWindow(
        "˫����Ϸ",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // ========== ����������Դ ========== //
    const char* menuImagePath = "1.bmp";
    const char* gameMapPath = "2.bmp";
    const char* playerPath = u8"���˹�.bmp";
    const char* obstaclePath = u8"�ϰ���.bmp";
    const char* monsterPath = u8"ԭʯ��.bmp";
    const char* doorPath = u8"��.bmp";
    const char* transitionPath = u8"����ȥ.png";//����ȥ
    const char* optionsPath = u8"�ĸ�ѡ��.png";//ѡ��
    const char* instructionPath = u8"HELP.png";//HELP
    const char* settingsPath = u8"SETTINGS.png";//SETTING
    const char* exitImagePath = u8"�˳�����.png";
    const char* gameMap2Texture = u8"��ͼ2.png";
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
    // ����
    musicManager.Load(MENU_MUSIC, u8"menu.wav");//�˵���������
    musicManager.Load(GAME_MUSIC, u8"game.wav");//������
    musicManager.Load(JUQING_MUSIC, u8"����.wav");//��������
    musicManager.Load(SHENGLI_MUSIC, u8"��Ϸʤ��.wav");//��Ϸʤ��
    musicManager.Load(FEIDADOU_MUSIC, u8"feidadou.wav");//�Ǵ�����
    musicManager.Load(NO_MUSIC, u8"no.wav");
    SDL_Texture* exitTexture = IMG_LoadTexture(renderer, exitImagePath);

    // �����������
    SDL_Texture* menuTexture = IMG_LoadTexture(renderer, menuImagePath);
    SDL_Texture* gameMapTexture = IMG_LoadTexture(renderer, gameMapPath);
    // ���ز�����͸��ͨ��
    SDL_Texture* playerTexture = IMG_LoadTexture(renderer, playerPath);
    SDL_SetTextureBlendMode(playerTexture, SDL_BLENDMODE_BLEND);
    SDL_Texture* obstacleTexture = IMG_LoadTexture(renderer, obstaclePath);
    SDL_Texture* monsterTexture = IMG_LoadTexture(renderer, monsterPath);
    SDL_Texture* doorTexture = IMG_LoadTexture(renderer, doorPath);
    // ��main������ͷ�����������
    SDL_Texture* transitionTexture = nullptr;
    SDL_Texture* optionsTexture = nullptr;
    SDL_Texture* instructionTexture = nullptr;
    SDL_Texture* settingsTexture = nullptr;
    // ������������
    transitionTexture = IMG_LoadTexture(renderer, transitionPath);
    optionsTexture = IMG_LoadTexture(renderer, optionsPath);
    instructionTexture = IMG_LoadTexture(renderer, instructionPath);
    settingsTexture = IMG_LoadTexture(renderer, settingsPath);

    // ������
    if (!menuTexture || !gameMapTexture || !playerTexture || !obstacleTexture || !monsterTexture || !doorTexture) {
        SDL_Log("��Դ����ʧ��: %s", IMG_GetError());
        return -1;
    }
    // ����Դ���غ���Ӽ��
    if (!transitionTexture || !optionsTexture || !instructionTexture || !settingsTexture) {
        SDL_Log("������Դ����ʧ��: %s", IMG_GetError());
        return -1;
    }

    // ��Ӵ�����
    if (!exitTexture) {
        SDL_Log("�˳��������ʧ��: %s", IMG_GetError());
        return -1;
    }
    // ���ع�����������
    for (const auto& path : cutscenePaths) {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path);
        if (!tex) {
            SDL_Log("����������Դ����ʧ��: %s", IMG_GetError());
            return -1;
        }
        cutsceneTextures.push_back(tex);
    }

    // ========== ��ʼ����Ϸ���� ========== //
    GameObject player = {
        playerTexture,
       {100, GROUND_Y - 50 , 50, 50}, // ��ʼλ���ڵ����ϣ����� Y=470����ɫ�� 80��
        0,
        0,
        false,
        {110, GROUND_Y - 45, 30, 45}
    };

    GameObject door = {
        doorTexture,
        {3000, GROUND_Y - 100, 80, 100}, // �ŵ�λ��
        0,
        0,
        false,
            {3000, GROUND_Y - 100, 80, 100}
    };

    std::vector<SDL_Rect> obstacles = {
        {500, GROUND_Y - 50, 50, 50},// �ϰ���λ��
        {550, GROUND_Y - 50, 50, 50},
        {600, GROUND_Y - 50, 50, 50},
        {650, GROUND_Y - 50, 50, 50},
        {550, GROUND_Y - 100, 50, 50},
        {600, GROUND_Y - 100, 50, 50},
        //�ڶ�������׶
         {800, GROUND_Y - 50, 50, 50},
         {850, GROUND_Y - 50, 50, 50},
         {900, GROUND_Y - 50, 50, 50},
         {850, GROUND_Y - 100, 50, 50},

         {1200, GROUND_Y - 50, 50, 50},
         {1250, GROUND_Y - 100, 50, 50},
         {1300, GROUND_Y - 100, 50, 50},
         {1250, GROUND_Y - 50, 50, 50},
         {1300, GROUND_Y - 50, 50, 50},
         //�Ƚϸߵ�ƽ��

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

    SDL_Rect camera = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }; // �������ʼ��
    // ========== ��Ϸ��ѭ�� ========== //
    GameState currentState = MENU;
    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        // �¼�����
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            // �˵��������
            if (currentState == MENU && event.type == SDL_MOUSEBUTTONDOWN) {//���ڲ˵�������Ҽ�⵽�����
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY); // ��ȡ�������
                SDL_Rect startButton = { 500, 300, 200, 100 };//start����������꣬x��y����ȣ��߶�
                SDL_Point mousePos = { mouseX, mouseY }; // ���� SDL_Point ʵ��
                if (SDL_PointInRect(&mousePos, &startButton)) { // �������Ƿ���start��ť������
                    currentState = TRANSITION;
                    transitionStartTime = SDL_GetTicks(); // ��¼���ɿ�ʼʱ��
                }
            }

            // ѡ�����������
            if (currentState == OPTIONS && event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                SDL_Point mousePos = { mouseX, mouseY };

                for (int i = 0; i < 4; ++i) {
                    if (SDL_PointInRect(&mousePos, &optionRects[i])) {
                        switch (i) {
                        case 0: currentState = GAME; break;    // ��Ϸ��ͼ
                        case 1: currentState = INSTRUCTION; break;//HELP
                        case 2: currentState = SETTINGS; break;//SETTINGS
                            // �޸�ѡ��������
                        case 3:
                            currentState = EXIT_SCREEN;  // ԭ"�˳���Ϸ"��Ϊ�����˳�����
                            transitionStartTime = SDL_GetTicks(); // ��¼�˳����濪ʼʱ��
                            break;

                        }
                        break;
                    }
                }
            }
            // ״̬����
            switch (currentState)
            {
            case TRANSITION:
                // ���ɻ�����ʾ0.5��
                if (SDL_GetTicks() - transitionStartTime > 500) {
                    currentState = OPTIONS;
                }
                break;
            case EXIT_SCREEN:
                // ��ʾ1����˳�
                if (SDL_GetTicks() - transitionStartTime > 1000) {
                    isRunning = false;
                }
                break;
                // ��״̬���²�����ӣ���ѭ���ڣ�
            case CUTSCENE: {
                Uint32 elapsed = SDL_GetTicks() - cutsceneStartTime;
                // ÿ2000ms�л�һ��ͼƬ
                if (elapsed > 2000) {
                    currentCutsceneIndex++;
                    cutsceneStartTime = SDL_GetTicks();

                    // ����ͼƬ�������
                    if (currentCutsceneIndex >= cutsceneTextures.size()) {
                        // �л����ڶ��ŵ�ͼ�������Ѽ���gameMap2Texture��
                        gameMapTexture = IMG_LoadTexture(renderer, "D:\\SDL_Project\\SDL_Demo\\SDL_Demo\\��ͼ2.png");
                        currentState = GAME;

                        // �������λ�õ��µ�ͼ���
                        player.position = { 100, GROUND_Y, 50, 50 };
                        camera.x = 0;
                    }
                }
                break;
            }
            }
            // ����
            switch (currentState) {
            case MENU:
                musicManager.Play(MENU_MUSIC);//�˵�����
                break;
            case TRANSITION:
                musicManager.Play(MENU_MUSIC, 0); // ��ѭ��
                break;
            case OPTIONS:
                musicManager.Play(MENU_MUSIC);//�˵�����
                break;
            case GAME:
                musicManager.Play(FEIDADOU_MUSIC);//�Ǵ�
                break;
            case WIN:
                musicManager.Play(SHENGLI_MUSIC);//ʤ��
                break;
            case CUTSCENE:
                musicManager.Play(JUQING_MUSIC);//����
                break;
            default:
                musicManager.Stop();
            }

            // ========== ��Ϸ�߼����� ========== //
            if (currentState == GAME) {

                //���������
                int oldX = player.position.x;
                int oldY = player.position.y;

                // ��ɫ����
                const Uint8* keystates = SDL_GetKeyboardState(NULL);
                player.speedX = (keystates[SDL_SCANCODE_LEFT] ? -5 : 0) + (keystates[SDL_SCANCODE_RIGHT] ? 5 : 0);
                player.position.x += player.speedX;
                player.collider.x = player.position.x + 10;

                // ˮƽ��ײ���
                for (auto& obs : obstacles) {
                    if (SDL_HasIntersection(&player.collider, &obs)) {
                        player.position.x = oldX;
                        player.collider.x = oldX + 10; // ���ﲻ��ƫ����
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

                // ��ֱ��ײ���
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
                // ���������
                camera.x = player.position.x - WINDOW_WIDTH / 2;
                camera.x = SDL_clamp(camera.x, 0, 3000 - WINDOW_WIDTH);

                // ʤ�����
                if (SDL_HasIntersection(&player.position, &door.position)) {
                    currentState = CUTSCENE;
                    currentCutsceneIndex = 0;
                    cutsceneStartTime = SDL_GetTicks();  // ��¼����������ʼʱ��
                }
            }

        }

        // ��Ⱦ����
        SDL_RenderClear(renderer);
        switch (currentState) {
            //��Ⱦ�˵�
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
                        // ����Ⱦ�������case
        case CUTSCENE: {
            if (currentCutsceneIndex < cutsceneTextures.size()) {
                SDL_RenderCopy(renderer, cutsceneTextures[currentCutsceneIndex], NULL, NULL);
            }
            break;
        }
        case GAME: {
            // ��Ⱦ��ͼ
            SDL_RenderCopy(renderer, gameMapTexture, NULL, NULL);

            // ��Ⱦ�ϰ���
            for (auto& obs : obstacles) {
                SDL_Rect renderPos = { obs.x - camera.x, obs.y, obs.w, obs.h };
                SDL_RenderCopy(renderer, obstacleTexture, NULL, &renderPos);
            }

            // ��Ⱦ��ɫ
            SDL_Rect playerPos = { player.position.x - camera.x, player.position.y, 50, 50 };
            SDL_RenderCopy(renderer, playerTexture, NULL, &playerPos);

            // ��Ⱦ����
            for (auto& monster : monsters) {
                SDL_Rect monsterPos = { monster.position.x - camera.x, monster.position.y, 40, 40 };
                SDL_RenderCopy(renderer, monsterTexture, NULL, &monsterPos);
            }

            // ��Ⱦ��
            SDL_Rect doorPos = { door.position.x - camera.x, door.position.y, 80, 100 };
            SDL_RenderCopy(renderer, doorTexture, NULL, &doorPos);
            break;

        }
                 //��Ⱦʤ������
        case WIN: {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderClear(renderer);
            break;
        }

        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    // ========== ��Դ���� ========== //
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