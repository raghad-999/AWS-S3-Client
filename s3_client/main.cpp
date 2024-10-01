/** Compilation Instruction: g++ -std=c++11 main.cpp s3_client.cpp -o s3client_app -laws-cpp-sdk-core -laws-cpp-sdk-s3 **/

#include "s3_client.hpp"
#include <aws/core/Aws.h>
#include <iostream>

int main()
{
    Aws::SDKOptions options;
    Aws::InitAPI(options);

    // std::string endpoint, access_key, secret_key;
    // std::cout << "Enter MinIO endpoint: ";
    // std::cin >> endpoint;
    // std::cout << "Enter access key: ";
    // std::cin >> access_key;
    // std::cout << "Enter secret key: ";
    // std::cin >> secret_key;

    // S3Client s3_client(endpoint, access_key, secret_key);

    S3Client s3_client("http://127.0.0.1:9000", "minioadmin", "minioadmin");

    int choice;
    do {
        std::cout << "\n--- S3 Menu ---" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "1. List buckets" << std::endl;
        std::cout << "2. Create bucket" << std::endl;
        std::cout << "3. Delete bucket" << std::endl;
        std::cout << "4. Upload file" << std::endl;
        std::cout << "5. List objects in a bucket" << std::endl;
        std::cout << "6. Download object" << std::endl;
        std::cout << "7. Delete object" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 0:
                std::cout << "Exiting...." << std::endl;
                break;
            case 1:
                s3_client.list_buckets();
                break;
            case 2:
                s3_client.create_bucket();
                break;
            case 3:
                s3_client.delete_bucket();
                break;
            case 4:
                s3_client.upload_file();
                break;
            case 5:
                s3_client.list_objects();
                break;
            case 6:
                s3_client.download_file();
                break;
            case 7:
                s3_client.delete_object();
                break;
            default:
                std::cerr << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 0);

    Aws::ShutdownAPI(options);
    return 0;
}
