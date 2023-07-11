#include <iostream>
#include <vector>
#include <algorithm>

#define NMAX 10000
#define KMAX 10000

using namespace std;

struct Item{
    uint32_t id, width, height;
    uint32_t truck_id, corner_x, corner_y;
    bool rotated = false;
};

struct Free_Rectangle{
    uint32_t corner_x, corner_y, width, height;
};

struct Suitable_Free_Rec_Result{
    Free_Rectangle suitable_rec;
    uint32_t rec_id;
    bool isRotated; 
    bool isExist;
};

struct Truck{
    uint32_t id, width, height;
    uint32_t cost;
    vector<Free_Rectangle> list_free_rectangles;
    vector<Item> list_items;
};

uint32_t n_items, n_trucks;
Item item[NMAX];
Truck truck[KMAX];

bool compareItem(Item a, Item b){
    if(a.height == b.height) return a.width > b.width;
    return a.height > b.height;
}

bool compareItemByID(Item a, Item b){
    return a.id < b.id;
}

void rotateItem(Item &item){
    item.rotated = 1 - item.rotated;
    uint32_t temp = item.width;
    item.width = item.height;
    item.height = temp;
}

bool compareTruck(Truck a, Truck b){
    float value_a = (a.width * a.height)/(a.cost);
    float value_b = (b.width * b.height)/(b.cost);
    if(value_a == value_b) 
        return (a.width * a.height)/(a.width + a.height) > (b.width * b.height)/(b.width + b.height);
    return value_a > value_b;
}

bool isFitToFreeRectangle(Free_Rectangle rec, Item item, bool rotated){
    if(rotated)
        return (item.width <= rec.height) && (item.height <= rec.width);
    return (item.width <= rec.width) && (item.height <= rec.height);
}

pair <uint32_t, uint32_t> getFreeRectangleScore(Free_Rectangle rec, Item item, bool rotated){
    pair <uint32_t, uint32_t> score;
    uint32_t remain_width = rec.width - (rotated ? item.height : item.width);
    uint32_t remain_height = rec.height - (rotated ? item.width : item.height);
    score.first = min(remain_width, remain_height);
    score.second = max(remain_width, remain_height);
    return score;
}

bool compareFreeRectangleScore(pair <uint32_t, uint32_t> a, pair <uint32_t, uint32_t> b){
    if(a.first == b.first) return a.second < b.second;
    return a.first < b.first;
}

Suitable_Free_Rec_Result findSuitableFreeRectangle(Truck truck, Item item){
    Suitable_Free_Rec_Result suitable_rec;
    suitable_rec.isRotated = false;
    suitable_rec.isExist = false;
    suitable_rec.rec_id = 0;

    pair<uint32_t, uint32_t> best_score = {UINT32_MAX, UINT32_MAX};

    const uint32_t n_rectangles = truck.list_free_rectangles.size();
    pair<uint32_t, uint32_t> rec_score;
    for (uint32_t i=0; i<n_rectangles; ++i) {
        Free_Rectangle rec = truck.list_free_rectangles[i];
        
        // Not rotate case
        rec_score = getFreeRectangleScore(rec, item, 0);
        if (isFitToFreeRectangle(rec, item, 0) && compareFreeRectangleScore(rec_score, best_score)) {
            best_score = rec_score;
            suitable_rec.suitable_rec = move(rec);
            suitable_rec.rec_id = i;
            suitable_rec.isRotated = false;
            suitable_rec.isExist = true;
        }

        // Rotate case
        rec_score = getFreeRectangleScore(rec, item, 1);
        if (isFitToFreeRectangle(rec, item, 1) && compareFreeRectangleScore(rec_score, best_score)) {
            best_score = rec_score;
            suitable_rec.suitable_rec = move(rec);
            suitable_rec.rec_id = i;
            suitable_rec.isRotated = true;
            suitable_rec.isExist = true;
        }
    }

    return suitable_rec;
}

vector <Free_Rectangle> splitFreeRectangle(Free_Rectangle rec, Item item){
    vector <Free_Rectangle> list_free_rectangles;
    Free_Rectangle new_free_rec;

    // Vertical split to maximize the right corner free_rec
    if(item.width < rec.width){
        new_free_rec.width      = rec.width - item.width;
        new_free_rec.height     = rec.height;
        new_free_rec.corner_x   = rec.corner_x + item.width;
        new_free_rec.corner_y   = rec.corner_y;
        list_free_rectangles.push_back(new_free_rec);
    }

    // Horizontal split to maximize the top corner free_rec
    if(item.height < rec.height){
        new_free_rec.width      = rec.width;
        new_free_rec.height     = rec.height - item.height;
        new_free_rec.corner_x   = rec.corner_x;
        new_free_rec.corner_y   = rec.corner_y + item.height;
        list_free_rectangles.push_back(new_free_rec);
    }

    return list_free_rectangles;
}

bool isItemIntersecFreeRectangle(Free_Rectangle rec, Item item){
    if(item.corner_x >= rec.corner_x + rec.width)
        return false;
    if(item.corner_y >= rec.corner_y + rec.height)
        return false;
    if(item.corner_x + item.width <= rec.corner_x)
        return false;
    if(item.corner_y + item.height <= rec.corner_y)
        return false;
    return true;
}

Free_Rectangle getOverlappingRectangle(Free_Rectangle rec, Item item){
    Free_Rectangle overlap_rec;
    overlap_rec.corner_x = max(rec.corner_x, item.corner_x);
    overlap_rec.corner_y = max(rec.corner_y, item.corner_y);

    overlap_rec.width = min(rec.corner_x + rec.width, item.corner_x + item.width) - overlap_rec.corner_x;
    overlap_rec.height = min(rec.corner_y + rec.height, item.corner_y + item.height) - overlap_rec.corner_y;

    return overlap_rec;
}

vector<Free_Rectangle> splitOverlappingRectangle(Free_Rectangle initial_rec, Free_Rectangle overlap_rec){
    vector<Free_Rectangle> list_free_rectangles;
    Free_Rectangle new_free_rec;
    // Vertical split to maximize the left corner free_rec
    if(overlap_rec.corner_x > initial_rec.corner_x){
        new_free_rec.corner_x   = initial_rec.corner_x;
        new_free_rec.corner_y   = initial_rec.corner_y;
        new_free_rec.width      = overlap_rec.corner_x - new_free_rec.corner_x;
        new_free_rec.height     = initial_rec.height;
        list_free_rectangles.push_back(new_free_rec);
    }

    // Vertical split to maximize the right corner free_rec
    if(overlap_rec.corner_x + overlap_rec.width < initial_rec.corner_x + initial_rec.width){
        new_free_rec.corner_x   = overlap_rec.corner_x + overlap_rec.width;
        new_free_rec.corner_y   = initial_rec.corner_y;
        new_free_rec.width      = initial_rec.corner_x + initial_rec.width - new_free_rec.corner_x;
        new_free_rec.height     = initial_rec.height;
        list_free_rectangles.push_back(new_free_rec);
    }

    // Horizontal split to maximize the bottom corner free_rec
    if(overlap_rec.corner_y > initial_rec.corner_y){
        new_free_rec.corner_x   = initial_rec.corner_x;
        new_free_rec.corner_y   = initial_rec.corner_y;
        new_free_rec.width      = initial_rec.width;
        new_free_rec.height     = overlap_rec.corner_y - new_free_rec.corner_y;
        list_free_rectangles.push_back(new_free_rec);
    }

    // Horizontal split to maximize the top corner free_rec
    if(overlap_rec.corner_y + overlap_rec.height < initial_rec.corner_y + initial_rec.height){
        new_free_rec.corner_x   = initial_rec.corner_x;
        new_free_rec.corner_y   = overlap_rec.corner_y + overlap_rec.height;
        new_free_rec.width      = initial_rec.width;
        new_free_rec.height     = initial_rec.corner_y + initial_rec.height - new_free_rec.corner_y;
        list_free_rectangles.push_back(new_free_rec);
    }

    return list_free_rectangles;
}

// Check if a free_rec is fully covered by another free_rec
bool checkCovered(Free_Rectangle rec_covering, Free_Rectangle rec_covered){
    // Not intersect
    if(rec_covered.corner_x > rec_covering.corner_x + rec_covering.width) return false;
    if(rec_covered.corner_y > rec_covering.corner_y + rec_covering.height) return false;
    if(rec_covered.corner_x + rec_covered.width < rec_covering.corner_x) return false;
    if(rec_covered.corner_y + rec_covered.height < rec_covering.corner_y) return false;

    // Intersect but not fully covered
    if(rec_covered.corner_x < rec_covering.corner_x) return false;
    if(rec_covered.corner_y < rec_covering.corner_y) return false;
    if(rec_covered.corner_x + rec_covered.width > rec_covering.corner_x + rec_covering.width) return false;
    if(rec_covered.corner_y + rec_covered.height > rec_covering.corner_y + rec_covering.height) return false;

    return true;
}

// Remove the covered free_rec
void removeCoveredRectangles(Truck &truck){
    for(uint32_t i = 0; i < truck.list_free_rectangles.size(); ++i){
        Free_Rectangle first = truck.list_free_rectangles[i];
        for(uint32_t j = i + 1; j < truck.list_free_rectangles.size(); ++j){
            Free_Rectangle second = truck.list_free_rectangles[j];
            // If rec i cover rec j then delete rec j
            if(checkCovered(first, second)){
                truck.list_free_rectangles.erase(truck.list_free_rectangles.begin() + j);
                --j;
                continue;
            }
            // If rec j cover rec i then delete rec i
            if(checkCovered(second, first)){
                truck.list_free_rectangles.erase(truck.list_free_rectangles.begin() + i);
                --i;
                break;
            }
        }
    }
}

void removeOverlapping(Truck &truck, Item item){
    for(uint32_t i = 0; i < truck.list_free_rectangles.size(); ++i){
        Free_Rectangle rec = truck.list_free_rectangles[i];
        if(isItemIntersecFreeRectangle(rec, item)){
            Free_Rectangle overlap_rec = getOverlappingRectangle(rec, item);
            vector <Free_Rectangle> new_rec = splitOverlappingRectangle(rec, overlap_rec);
            truck.list_free_rectangles.erase(truck.list_free_rectangles.begin() + i);
            for(auto free_rec : new_rec){
                truck.list_free_rectangles.push_back(free_rec);
            }
            --i;
        }
    }
    removeCoveredRectangles(truck);
}

// Check if item find a possible free_rec in the truck for inserting process
bool tryPlaceItemToTruck(Truck &truck, Item &item){
    Suitable_Free_Rec_Result suitable_rec = findSuitableFreeRectangle(truck, item);

    if(!suitable_rec.isExist) return false;
    Free_Rectangle best_rec = suitable_rec.suitable_rec;

    // Add the item into the choosen free_rec
    item.truck_id = truck.id;
    item.corner_x = best_rec.corner_x;
    item.corner_y = best_rec.corner_y;
    if(suitable_rec.isRotated) 
        rotateItem(item);
    
    truck.list_items.push_back(item);

    // Replace the used free_rec by the new splited rec(s)
    truck.list_free_rectangles.erase(truck.list_free_rectangles.begin() + suitable_rec.rec_id);
    vector<Free_Rectangle> new_recs = splitFreeRectangle(best_rec, item);
    for(Free_Rectangle rec : new_recs){
        truck.list_free_rectangles.push_back(rec);
    }

    removeOverlapping(truck, item);

    return true;
}

void solve(){
    for(uint32_t i = 1; i <= n_items; ++i){
        for(uint32_t j = 1; j <= n_trucks; ++j)
            if(tryPlaceItemToTruck(truck[j],item[i])) 
                break;
    }
}

void readInput(){
    // freopen("INPUT.txt", "r", stdin);
    cin >> n_items >> n_trucks;

    for(uint32_t i=1; i<=n_items; ++i){
        cin >> item[i].width >> item[i].height;
        item[i].id = i;
        if(item[i].width > item[i].height) rotateItem(item[i]);
    }

    for(uint32_t i=1; i<=n_trucks; ++i){
        cin >> truck[i].width >> truck[i].height >> truck[i].cost;
        truck[i].id = i;

        Free_Rectangle first_rec;
        first_rec.width = truck[i].width;
        first_rec.height = truck[i].height;
        first_rec.corner_x = 0;
        first_rec.corner_y = 0;
        truck[i].list_free_rectangles.push_back(first_rec);
    }

    sort(item + 1, item + n_items + 1, compareItem);
    sort(truck + 1, truck + n_trucks + 1, compareTruck);
}

void printSolution(){
    sort(item + 1, item + n_items + 1, compareItemByID);
    for(uint32_t i=1; i<=n_items; ++i){
        cout << item[i].id << ' ' << item[i].truck_id << ' ' << item[i].corner_x << ' ' << item[i].corner_y << ' ' << item[i].rotated << '\n';
    }
}

int main(){
    ios_base::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);

    readInput();
    solve();
    printSolution();
    return 0;
}

