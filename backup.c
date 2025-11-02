
#include <stdio.h>

#define TABLE_SIZE 8  // 8-entry table for branch predictor

// Convert counter value into human-readable state
const char* getState(int counter) {
    if (counter <= 1) return "Strongly Not Taken";
    else if (counter <= 3) return "Weakly Not Taken";
    else if (counter <= 5) return "Weakly Taken";
    else return "Strongly Taken";
}

// Update a counter with saturating logic
void updateCounter(int *counter, char actual) {
    if (actual == 'T' && *counter < 7) (*counter)++;
    else if (actual == 'N' && *counter > 0) (*counter)--;
}

// Simulate one member’s pattern
int simulateMember(int member, char *pattern, char *name) {
    int counters[TABLE_SIZE];
    for (int i = 0; i < TABLE_SIZE; i++) counters[i] = 3;  // Weakly Not Taken

    int member_correct = 0, member_total = 0;

    printf("\n--- %s ---\n", name);
    printf("Pattern: %s\n", pattern);
    printf("Step | Index | Counter | Prediction | Actual | Correct?\n");
    printf("-------------------------------------------------------\n");

    for (int i = 0; pattern[i] != '\0'; i++) {
        int index = i % TABLE_SIZE;
        char actual = pattern[i];
        char prediction = (counters[index] >= 4) ? 'T' : 'N';
        int correct = (prediction == actual);

        // Print step info
        printf(" %2d  |  %2d   |    %d    |     %c       |   %c    |   %s\n",
               i + 1, index, counters[index], prediction, actual,
               correct ? "Yes" : "No");

        // Update stats
        member_total++;
        if (correct) member_correct++;

        // Update counter
        updateCounter(&counters[index], actual);
    }

    // Show accuracy
    printf("\nAccuracy for %s: %.2f%%\n",
           name, (member_correct * 100.0) / member_total);

    // Show final counters
    printf("Final Counter States:\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("Counter %d: %d (%s)\n", i, counters[i], getState(counters[i]));
    }
    printf("=======================================================\n");

    return member_correct;
}

int main() {
    // Branch patterns
    char *patterns[] = {
        "TTTTTTTT",       // Always Taken
        "NNNNNNNN",       // Always Not Taken
        "TNTNTNTN",       // Alternating
        "TTTNTTTNTTTTN"   // Biased ~75% Taken
    };

    char *names[] = {
        "Member 1: Always Taken",
        "Member 2: Always Not Taken",
        "Member 3: Alternating",
        "Member 4: Biased Taken"
    };

    int total_predictions = 0, total_correct = 0;

    printf("===== TEAM BRANCH PREDICTOR SIMULATION =====\n");
    printf("3-bit Saturating Counter Predictor (0–7)\n");
    printf("Taken if counter >= 4, Not Taken if counter <= 3\n");
    printf("============================================\n");

    for (int member = 0; member < 4; member++) {
        int len = 0;
        while (patterns[member][len] != '\0') len++;

        int member_correct = simulateMember(member, patterns[member], names[member]);
        total_predictions += len;
        total_correct += member_correct;
    }

    // Team results
    printf("\n===== TEAM RESULTS =====\n");
    printf("Total Predictions: %d\n", total_predictions);
    printf("Correct Predictions: %d\n", total_correct);
    printf("Overall Accuracy: %.2f%%\n",
           (total_correct * 100.0) / total_predictions);

    return 0;
}
