#include "include/utility.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <regex>

TEST(UtilityTest, HandleInvalidRegex) {
    // Invalid regex pattern
    std::string invalidPattern = ".*\\[a-z"; // Missing closing bracket

    // Create a temporary test directory
    std::string testDirectory = "test_data";
    std::filesystem::create_directory(testDirectory);
    std::ofstream(testDirectory + "/file1.txt").close();

    // Attempt to convert invalid regex pattern
    try {
        std::regex invalidRegex = convertToRegexPattern(invalidPattern);

        // Check if the returned regex is valid
        if (invalidRegex.mark_count() == 0) {
            std::vector<std::string> matchingFiles =
                getMatchingDataFiles(invalidRegex, testDirectory);

            // Expect no matches since the regex is empty
            ASSERT_TRUE(matchingFiles.empty())
                << "Expected no matches with an empty regex.";
            SUCCEED(); // Empty regex means invalid regex was handled
        } else {
            FAIL() << "Expected an empty regex due to invalid pattern.";
        }
    } catch (const std::regex_error& e) {
        // If exception is thrown during regex creation
        FAIL() << "convertToRegexPattern should handle invalid regex without "
                  "throwing exceptions.";
    }

    // Clean up
    std::filesystem::remove_all(testDirectory);
}

TEST(UtilityTest, ConvertToRegexAndGetMatchingFiles) {
    // Set up test directory and files
    std::string testDirectory = "test_data";
    std::filesystem::create_directory(testDirectory);
    std::ofstream(testDirectory + "/file1.TXT").close();
    std::ofstream(testDirectory + "/file2.log").close();
    std::ofstream(testDirectory + "/file3.TxT").close();
    std::ofstream(testDirectory + "/file4.txt").close();

    // Define the string pattern and convert it to regex
    std::string stringPattern = ".*\\.txt$";
    std::regex pattern = convertToRegexPattern(stringPattern);

    // Call the function
    std::vector<std::string> matchingFiles =
        getMatchingDataFiles(pattern, testDirectory);

    // Verify the results
    ASSERT_EQ(matchingFiles.size(), 3); // Expect 3 .txt files, case insensitive
    EXPECT_NE(
        std::find(matchingFiles.begin(), matchingFiles.end(), "file1.TXT"),
        matchingFiles.end());
    EXPECT_NE(
        std::find(matchingFiles.begin(), matchingFiles.end(), "file3.TxT"),
        matchingFiles.end());
    EXPECT_NE(
        std::find(matchingFiles.begin(), matchingFiles.end(), "file4.txt"),
        matchingFiles.end());

    // Clean up
    std::filesystem::remove_all(testDirectory);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
