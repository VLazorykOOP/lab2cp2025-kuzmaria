#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <cmath>
#include <mutex>

const int V = 1;             // Швидкість руху
const int N = 3;             // Інтервал зміни напрямку для трутня
std::mutex mtx;              // М'ютекс для синхронного виводу

// === Клас Бджоли-робочої ===
class WorkerBee {
public:
    WorkerBee(int x, int y) : startX(x), startY(y), posX(x), posY(y), goBack(false) {}

    void operator()() {
        while (true) {
            moveStep();
            printPosition();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

private:
    int startX, startY;
    float posX, posY;
    bool goBack;

    void moveStep() {
        int targetX = goBack ? startX : 0;
        int targetY = goBack ? startY : 0;

        float dx = targetX - posX;
        float dy = targetY - posY;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < V) {
            goBack = !goBack;
            return;
        }

        posX += V * dx / dist;
        posY += V * dy / dist;
    }

    void printPosition() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[WorkerBee] Position: (" << posX << ", " << posY << ")\n";
    }
};

// === Клас Трутня ===
class Drone {
public:
    Drone(int x, int y) : posX(x), posY(y), gen(rd()), dis(-1.0, 1.0) {
        randomizeDirection();
    }

    void operator()() {
        int counter = 0;
        while (true) {
            if (counter++ % N == 0) {
                randomizeDirection();
            }

            posX += V * dirX;
            posY += V * dirY;

            printPosition();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

private:
    float posX, posY;
    float dirX, dirY;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

    void randomizeDirection() {
        dirX = dis(gen);
        dirY = dis(gen);
        float len = std::sqrt(dirX * dirX + dirY * dirY);
        dirX /= len;
        dirY /= len;
    }

    void printPosition() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "[Drone] Position: (" << posX << ", " << posY << ")\n";
    }
};

// === main ===
int main() {
    WorkerBee bee(10, 10);
    Drone drone(5, 5);

    std::thread t1(std::ref(bee));
    std::thread t2(std::ref(drone));

    t1.join();
    t2.join();

    return 0;
}
