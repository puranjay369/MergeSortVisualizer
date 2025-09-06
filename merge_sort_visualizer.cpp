#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>
#include <queue>
using namespace std;

struct VisualizationState {
    vector<int> array;
    string operation;
    vector<int> leftIndices;
    vector<int> rightIndices;
    vector<int> sortedIndices;
    int stepNumber;
    
    VisualizationState(vector<int> arr, string op, vector<int> left = {}, vector<int> right = {}, vector<int> sorted = {}, int step = 0)
        : array(arr), operation(op), leftIndices(left), rightIndices(right), sortedIndices(sorted), stepNumber(step) {}
};

class CompleteMergeSortVisualizer {
private:
    sf::RenderWindow window;
    vector<int> originalArray;
    queue<VisualizationState> animationQueue;
    
    // Window properties
    float windowWidth = 1200;
    float windowHeight = 800;
    float barWidth;
    float maxHeight = 500;
    int maxValue;
    
    // Animation control
    bool isPlaying = false;
    bool isPaused = true;
    int currentStep = 0;
    int totalSteps = 0;
    int animationSpeed = 800; // milliseconds
    sf::Clock animationClock;
    
    // Statistics
    int comparisons = 0;
    int arrayAccesses = 0;
    int mergeOperations = 0;
    
    // UI Elements
    sf::Font font;
    sf::Text titleText, statusText, controlsText, statsText, arrayText;
    sf::RectangleShape headerRect, controlPanelRect;
    
    // Colors
    sf::Color BACKGROUND = sf::Color(25, 25, 35);
    sf::Color HEADER_BG = sf::Color(40, 40, 55);
    sf::Color PANEL_BG = sf::Color(35, 35, 50);
    sf::Color BAR_NORMAL = sf::Color(70, 130, 255);
    sf::Color BAR_LEFT = sf::Color(255, 80, 80);
    sf::Color BAR_RIGHT = sf::Color(80, 255, 80);
    sf::Color BAR_SORTED = sf::Color(255, 200, 50);
    sf::Color TEXT_PRIMARY = sf::Color::White;
    sf::Color TEXT_ACCENT = sf::Color(100, 200, 255);
    
public:
    CompleteMergeSortVisualizer(vector<int> arr) : originalArray(arr) {
        // Adjust window size based on array size for better visualization
        if (arr.size() > 100) {
            windowWidth = 1600; // Wider window for more bars
            windowHeight = 900;
        }
        
        window.create(sf::VideoMode(windowWidth, windowHeight), "Complete Merge Sort Visualizer");
        window.setFramerateLimit(60);
        
        barWidth = max(1.0f, (windowWidth - 100) / (float)arr.size()); // Ensure minimum width
        maxValue = *max_element(arr.begin(), arr.end());
        
        // Adjust animation speed for larger arrays (faster by default)
        if (arr.size() > 50) {
            animationSpeed = 200; // Faster for large arrays
        }
        
        setupUI();
        generateVisualizationSteps();
    }
    
    void setupUI() {
        // Try to get SFML's default font
        cout << "Setting up UI..." << endl;
        
        // In SFML 2.6.x, we need to load a font file or use system fonts differently
        // For now, let's try without font and see if text renders
        
        // Header background
        headerRect.setSize(sf::Vector2f(windowWidth, 100));
        headerRect.setFillColor(HEADER_BG);
        
        // Control panel
        controlPanelRect.setSize(sf::Vector2f(windowWidth, 80));
        controlPanelRect.setPosition(0, windowHeight - 80);
        controlPanelRect.setFillColor(PANEL_BG);
        
        // Set up text objects - they should work with SFML's internal default font
        // Title
        titleText.setString("Merge Sort Visualizer - Complete Edition");
        titleText.setCharacterSize(28);
        titleText.setFillColor(TEXT_ACCENT);
        titleText.setPosition(20, 20);
        
        // Status
        statusText.setCharacterSize(16);
        statusText.setFillColor(TEXT_PRIMARY);
        statusText.setPosition(20, 60);
        
        // Controls - Make this very visible
        controlsText.setString("SPACE: Play/Pause | R: Reset | UP/DOWN: Speed | N: New Array | ESC: Exit");
        controlsText.setCharacterSize(18);
        controlsText.setFillColor(sf::Color::Yellow); // Bright yellow
        controlsText.setPosition(20, windowHeight - 50);
        
        // Statistics
        statsText.setCharacterSize(14);
        statsText.setFillColor(TEXT_PRIMARY);
        statsText.setPosition(windowWidth - 300, 25);
        
        // Array display
        arrayText.setCharacterSize(12);
        arrayText.setFillColor(TEXT_PRIMARY);
        arrayText.setPosition(20, windowHeight - 25);
        
        // Only set font if we have one loaded
        // For SFML 2.6.x, we'll use default rendering
        // titleText.setFont(font);
        // statusText.setFont(font);
        // controlsText.setFont(font);
        // statsText.setFont(font);
        // arrayText.setFont(font);
        
        updateUI();
    }
    
    void generateVisualizationSteps() {
        vector<int> arr = originalArray;
        totalSteps = 0;
        
        // Clear previous steps
        while (!animationQueue.empty()) animationQueue.pop();
        
        // Initial state
        animationQueue.push(VisualizationState(arr, "Initial unsorted array", {}, {}, {}, totalSteps++));
        
        // Generate merge sort steps
        recordMergeSort(arr, 0, arr.size() - 1);
        
        // Final state
        vector<int> allSorted;
        for (int i = 0; i < arr.size(); i++) allSorted.push_back(i);
        animationQueue.push(VisualizationState(arr, "Sorting complete!", {}, {}, allSorted, totalSteps++));
        
        totalSteps = animationQueue.size();
    }
    
    void recordMergeSort(vector<int>& arr, int left, int right) {
        if (left < right) {
            // Show division
            vector<int> divisionIndices;
            for (int i = left; i <= right; i++) divisionIndices.push_back(i);
            animationQueue.push(VisualizationState(arr, 
                "Dividing subarray [" + to_string(left) + ".." + to_string(right) + "]", 
                divisionIndices, {}, {}, totalSteps++));
            
            int mid = left + (right - left) / 2;
            
            // Recursively sort left and right halves
            recordMergeSort(arr, left, mid);
            recordMergeSort(arr, mid + 1, right);
            
            // Merge the sorted halves
            recordMerge(arr, left, mid, right);
        }
    }
    
    void recordMerge(vector<int>& arr, int left, int mid, int right) {
        // Show merge operation start
        vector<int> leftSubarray, rightSubarray;
        for (int i = left; i <= mid; i++) leftSubarray.push_back(i);
        for (int i = mid + 1; i <= right; i++) rightSubarray.push_back(i);
        
        animationQueue.push(VisualizationState(arr, 
            "Merging subarrays [" + to_string(left) + ".." + to_string(mid) + "] and [" + to_string(mid+1) + ".." + to_string(right) + "]",
            leftSubarray, rightSubarray, {}, totalSteps++));
        
        // Perform merge
        vector<int> temp;
        int i = left, j = mid + 1;
        
        while (i <= mid && j <= right) {
            comparisons++;
            if (arr[i] <= arr[j]) {
                temp.push_back(arr[i]);
                i++;
            } else {
                temp.push_back(arr[j]);
                j++;
            }
        }
        
        while (i <= mid) {
            temp.push_back(arr[i]);
            i++;
        }
        
        while (j <= right) {
            temp.push_back(arr[j]);
            j++;
        }
        
        // Copy back
        for (int k = 0; k < temp.size(); k++) {
            arr[left + k] = temp[k];
            arrayAccesses++;
        }
        
        mergeOperations++;
        
        // Show merge result
        vector<int> mergedIndices;
        for (int i = left; i <= right; i++) mergedIndices.push_back(i);
        animationQueue.push(VisualizationState(arr, 
            "Merged subarray [" + to_string(left) + ".." + to_string(right) + "]",
            {}, {}, mergedIndices, totalSteps++));
    }
    
    void updateUI() {
        // Update status
        string status = isPaused ? "Paused" : (isPlaying ? "Playing" : "Ready");
        statusText.setString("Status: " + status + " | Step: " + to_string(currentStep + 1) + "/" + to_string(totalSteps) + " | Speed: " + to_string(animationSpeed) + "ms");
        
        // Update statistics
        stringstream stats;
        stats << "Statistics:\n";
        stats << "Array Size: " << originalArray.size() << "\n";
        stats << "Comparisons: " << comparisons << "\n";
        stats << "Array Accesses: " << arrayAccesses << "\n";
        stats << "Merge Operations: " << mergeOperations;
        statsText.setString(stats.str());
        
        // Update array display (only for smaller arrays)
        if (originalArray.size() <= 20) {
            stringstream arrayStr;
            arrayStr << "Array: [";
            for (int i = 0; i < originalArray.size(); i++) {
                arrayStr << originalArray[i];
                if (i < originalArray.size() - 1) arrayStr << ", ";
            }
            arrayStr << "]";
            arrayText.setString(arrayStr.str());
        } else {
            // For large arrays, just show size and range
            int minVal = *min_element(originalArray.begin(), originalArray.end());
            int maxVal = *max_element(originalArray.begin(), originalArray.end());
            arrayText.setString("Array: " + to_string(originalArray.size()) + " elements, range: " + to_string(minVal) + "-" + to_string(maxVal));
        }
    }
    
    void drawVisualization(const VisualizationState& state) {
        window.clear(BACKGROUND);
        
        // Draw UI backgrounds
        window.draw(headerRect);
        window.draw(controlPanelRect);
        
        // Draw bars
        float startX = 50;
        float baseY = windowHeight - 160;
        
        for (int i = 0; i < state.array.size(); i++) {
            sf::RectangleShape bar;
            float barHeight = (state.array[i] / (float)maxValue) * maxHeight;
            
            bar.setSize(sf::Vector2f(max(1.0f, barWidth - 1), barHeight)); // Ensure minimum width
            float x = startX + i * barWidth;
            float y = baseY - barHeight;
            bar.setPosition(x, y);
            
            // Determine bar color
            sf::Color barColor = BAR_NORMAL;
            if (find(state.sortedIndices.begin(), state.sortedIndices.end(), i) != state.sortedIndices.end()) {
                barColor = BAR_SORTED;
            } else if (find(state.leftIndices.begin(), state.leftIndices.end(), i) != state.leftIndices.end()) {
                barColor = BAR_LEFT;
            } else if (find(state.rightIndices.begin(), state.rightIndices.end(), i) != state.rightIndices.end()) {
                barColor = BAR_RIGHT;
            }
            
            bar.setFillColor(barColor);
            
            // Only add outline for larger bars (performance optimization)
            if (barWidth > 3) {
                bar.setOutlineThickness(1);
                bar.setOutlineColor(sf::Color(255, 255, 255, 30));
            }
            
            window.draw(bar);
            
            // Only draw values for very small arrays (readability)
            if (state.array.size() <= 20 && barWidth > 15) {
                sf::Text valueText;
                valueText.setString(to_string(state.array[i]));
                valueText.setCharacterSize(10);
                valueText.setFillColor(TEXT_PRIMARY);
                
                sf::FloatRect bounds = valueText.getLocalBounds();
                valueText.setPosition(x + (barWidth - bounds.width) / 2, y - 20);
                window.draw(valueText);
            }
        }
        
        // Draw operation text
        sf::Text operationText;
        // operationText.setFont(font);  // Skip font for now
        operationText.setString(state.operation);
        operationText.setCharacterSize(18);
        operationText.setFillColor(TEXT_ACCENT);
        operationText.setPosition(20, 120);
        window.draw(operationText);
        
        // Draw UI text
        window.draw(titleText);
        window.draw(statusText);
        window.draw(controlsText);
        window.draw(statsText);
        window.draw(arrayText);
        
        window.display();
    }
    
    void generateRandomArray() {
        srand(time(nullptr));
        int arraySize = originalArray.size();
        
        // Generate new random values keeping the same size
        for (int i = 0; i < arraySize; i++) {
            originalArray[i] = rand() % maxValue + 1;
        }
        
        cout << "Generated new random array with " << arraySize << " elements" << endl;
        reset();
    }
    
    void reset() {
        currentStep = 0;
        isPlaying = false;
        isPaused = true;
        comparisons = 0;
        arrayAccesses = 0;
        mergeOperations = 0;
        
        generateVisualizationSteps();
        updateUI();
    }
    
    void run() {
        vector<VisualizationState> states;
        
        // Convert queue to vector for easier access
        queue<VisualizationState> tempQueue = animationQueue;
        while (!tempQueue.empty()) {
            states.push_back(tempQueue.front());
            tempQueue.pop();
        }
        
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                
                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                        case sf::Keyboard::Space:
                            isPlaying = !isPlaying;
                            isPaused = !isPaused;
                            break;
                        case sf::Keyboard::R:
                            reset();
                            break;
                        case sf::Keyboard::N:
                            generateRandomArray();
                            break;
                        case sf::Keyboard::Up:
                            // Adaptive speed adjustment based on array size
                            if (originalArray.size() > 100) {
                                animationSpeed = max(50, animationSpeed - 50);
                            } else {
                                animationSpeed = max(100, animationSpeed - 100);
                            }
                            break;
                        case sf::Keyboard::Down:
                            // Adaptive speed adjustment based on array size
                            if (originalArray.size() > 100) {
                                animationSpeed = min(1000, animationSpeed + 50);
                            } else {
                                animationSpeed = min(2000, animationSpeed + 100);
                            }
                            break;
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                    }
                    updateUI();
                }
            }
            
            // Animation logic
            if (isPlaying && !isPaused && animationClock.getElapsedTime().asMilliseconds() > animationSpeed) {
                currentStep++;
                if (currentStep >= states.size()) {
                    currentStep = states.size() - 1;
                    isPlaying = false;
                    isPaused = true;
                }
                animationClock.restart();
                updateUI();
            }
            
            // Draw current state
            if (!states.empty() && currentStep < states.size()) {
                drawVisualization(states[currentStep]);
            }
        }
    }
};

vector<int> generateRandomArray(int size, int maxVal = 500) {
    vector<int> arr;
    srand(time(nullptr));
    
    for (int i = 0; i < size; i++) {
        arr.push_back(rand() % maxVal + 1);
    }
    
    return arr;
}

int main() {
    cout << "=== Complete Merge Sort Visualizer ===" << endl;
    cout << "How many bars would you like to sort? (Recommended: 50-200): ";
    
    int arraySize;
    cin >> arraySize;
    
    // Validate input
    if (arraySize < 5) arraySize = 5;
    if (arraySize > 500) arraySize = 500; // Reasonable upper limit
    
    cout << "Generating " << arraySize << " random bars..." << endl;
    vector<int> initialArray = generateRandomArray(arraySize);
    
    cout << "\n=== CONTROLS ===" << endl;
    cout << "SPACE: Play/Pause animation" << endl;
    cout << "R: Reset to beginning" << endl;
    cout << "UP/DOWN arrows: Adjust speed" << endl;
    cout << "N: Generate new random array (same size)" << endl;
    cout << "ESC: Exit program" << endl;
    cout << "\nControls are also shown in the window. Close this console to focus on the graphics window." << endl;
    cout << "==========================================\n" << endl;
    
    CompleteMergeSortVisualizer visualizer(initialArray);
    visualizer.run();
    
    return 0;
}
