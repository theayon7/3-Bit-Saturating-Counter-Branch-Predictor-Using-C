#include <stdio.h>

int main() 
{
    // Test patterns for 4 team members
    char pattern1[] = "TTTTTTTT";           // Member 1: Always taken
    char pattern2[] = "NNNNNNNN";           // Member 2: Never taken  
    char pattern3[] = "TNTNTNTN";           // Member 3: Alternating
    char pattern4[] = "TTTNTTTNTTTTN";      // Member 4: Mostly taken
    
    char *patterns[] = {pattern1, pattern2, pattern3, pattern4};
    char *descriptions[] = {"Always Taken Loop", "Always Not Taken", 
                           "Alternating Pattern", "Biased Taken (75%)"};
    int counters[] = {3, 3, 3, 3};  // Start all at 3 (weakly not taken)
    int total_correct = 0, total_predictions = 0;

    
    printf("3-bit Saturating Counter (0-7: N=0-3, T=4-7)\n");
    printf("==========================================\n");
    
    // Test each member's pattern
    for (int member = 0; member < 4; member++) 
    {
        printf("\n=== Member %d Testing: %s ===\n", member + 1, descriptions[member]);
        printf("Branch Address: %d (Index: %d)\n", (member + 1) * 100, member);
        printf("Pattern: %s\n", patterns[member]);
        printf("Step | Counter | Prediction | Actual | Correct?\n");
        printf("---------------------------------------------\n");
        
        // Process each step in the pattern
        for (int i = 0; patterns[member][i] != '\0'; i++) 
        {
            char actual = patterns[member][i];
            char prediction = (counters[member] >= 4) ? 'T' : 'N';
            int correct = (prediction == actual);
            
            printf(" %2d  |   %d      |     %c      |   %c    |   %s\n",
                   i + 1, counters[member], prediction, actual,
                   correct ? "Yes" : "No");
            
            // Update statistics
            total_predictions++;
            if (correct) total_correct++;
            
            // Update counter (saturating)
            if (actual == 'T' && counters[member] < 7)
                counters[member]++;
            else if (actual == 'N' && counters[member] > 0)
                counters[member]--;
        }
    }
    
    // Team results
    printf("\n=== TEAM RESULTS ===\n");
    printf("Total Predictions: %d\n", total_predictions);
    printf("Correct Predictions: %d\n", total_correct);
    printf("Overall Accuracy: %.2f%%\n", (total_correct * 100.0) / total_predictions);
    
    // Final counter states
    printf("\nFinal Counter States:\n");
    printf("Branch Addr | Index | Counter | State\n");
    printf("------------------------------------\n");
    for (int i = 0; i < 4; i++) {
        printf("    %d     |   %d  |    %d    | %s\n", 
               (i + 1) * 100, i, counters[i],
               counters[i] >= 4 ? "Strongly Taken" : "Strongly Not Taken");
    }
    
    return 0;
}
