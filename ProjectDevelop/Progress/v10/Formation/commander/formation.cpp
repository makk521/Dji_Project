#include <iostream>
#include <vector>
#include <cmath>
#include "Commander.h"

 std::vector<std::array<double, 3>> Commander::calculate_formation(std::array<double, 3> center, double angle, int n_uavs, double distance, int cmd) {
        std::vector<std::array<double, 3>> targets;
        double angle_rad = angle * M_PI / 180.0; // 角度转为弧度
        double side_angle_rad = 2 * M_PI / 3; // 箭头排列120°

        for (int i = 1; i <= n_uavs; ++i) {
            Commander::UAV uav;
            uav.id = i;
            //uav.position = center;

            double offset, wing_offset;
            if (n_uavs % 2 == 0) {
                offset = (i - n_uavs / 2.0) * distance;
            } else {
                offset = (i - (n_uavs + 1) / 2.0) * distance;
            }

            std::array<double, 3> target;
            switch (cmd) {
                case 1: // 直线
                    target = {center[0] + offset * cos(angle_rad), center[1] + offset * sin(angle_rad), center[2]};
                    break;
                case 2: // 箭头  
                 
                    if (i == 1) { // 箭头尖端
                        target = center;
                    } else if (i % 2 == 0) { // 左
                        offset = (i + 1) / 2;
                        target = {center[0] + offset * distance * cos(-angle_rad + 7* M_PI/6  ), 
                                center[1] + offset * distance * sin(-angle_rad +  7* M_PI/6 ), 
                                center[2]};
                    } else { // 右
                       offset = i / 2;
                        target = {center[0] + offset * distance * cos(-angle_rad + 11*M_PI/6), 
                                center[1] + offset * distance * sin(-angle_rad +11*M_PI/6), 
                                center[2]};
                    }

                    // 如果是偶数最后一架无人机放队尾
                    if (n_uavs % 2 == 0 && i == n_uavs) {
                        offset = i / 2;
                            target = {center[0] + offset * distance * cos(-angle_rad + M_PI / 3), 
                                    center[1] + offset * distance * sin(-angle_rad + M_PI / 3), 
                                    center[2]};
                    
                    }
                    break;            
                
                case 3: 
                    {  // XYZ 间距都是 distance，投影在二维是一条斜线
                    
                        double dx = distance * cos(angle_rad); // X 轴上的间距
                        double dy = distance * sin(angle_rad); // Y 轴上的间距

                        // 计算每架无人机的位置
                        target = {center[0] + (i - 1) * dx, 
                                center[1] + (i - 1) * dy, 
                                center[2] + (i - 1) * distance}; // Z 轴上的间距也是 distance
                        break;
                    }
                
                default:
                    
                    break;
            }

            targets.push_back(target);
        }

        return targets;
    }


// int main() {
//     Commander cmd;
//     std::array<double, 3> center = {0.0, 0.0, 0.0}; // 中心点的三维坐标
//     double angle = 45;//这是与正东方向（x轴）的夹角！
//     int n_uavs = 7;
//     double distance = 1.0;

//     std::vector<std::array<double, 3>> targets = cmd.calculate_formation(center, angle, n_uavs, distance, 2);


//     // 打印目标点集合
//     std::cout << "Target Points:" << std::endl;
//     for (const auto& target : targets) {
//         std::cout << "(" << target[0] << ", " << target[1] << ", " << target[2] << ")" << std::endl;
//     }

//     return 0;
// }

