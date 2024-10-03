#include "s3_client.hpp"
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/ListBucketsRequest.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>

#include <fstream>
#include <iostream>

S3Client::S3Client(const std::string& endpoint, const std::string& access_key, const std::string& secret_key)
{
    // Initialize AWS API and create credentials provider
    auto credentialsProvider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>("MinioCredentialsProvider", access_key.c_str(), secret_key.c_str());

    Aws::S3::S3ClientConfiguration clientConfiguration;
    clientConfiguration.endpointOverride = endpoint;

    // Create S3 client
    s3_client = Aws::S3::S3Client(credentialsProvider, nullptr, clientConfiguration);
}

S3Client::~S3Client()
{
    // Shutdown AWS API
    Aws::ShutdownAPI({});
}

void S3Client::list_buckets()
{
    auto outcome = s3_client.ListBuckets();
    if (outcome.IsSuccess())
    {
        std::cout << "Buckets:" << std::endl;
        for (const auto& bucket : outcome.GetResult().GetBuckets())
        {
            std::cout << "  - " << bucket.GetName() << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to list buckets: " << outcome.GetError().GetMessage() << std::endl;
    }
}

void S3Client::create_bucket()
{
    std::string bucket_name;
    std::cout << "Enter bucket name: ";
    std::cin >> bucket_name;

    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucket_name.c_str());

    auto outcome = s3_client.CreateBucket(request);
    if (outcome.IsSuccess())
    {
        std::cout << "Successfully created bucket: " << bucket_name << std::endl;
    }
    else
    {
        std::cerr << "Failed to create bucket: " << outcome.GetError().GetMessage() << std::endl;
    }
}

bool S3Client::delete_all_objects_in_bucket(const std::string& bucket_name)
{
    Aws::S3::Model::ListObjectsRequest list_objects_request;
    list_objects_request.WithBucket(bucket_name.c_str());

    auto list_objects_outcome = s3_client.ListObjects(list_objects_request);
    if (!list_objects_outcome.IsSuccess())
    {
        std::cerr << "Failed to list objects in bucket: " << list_objects_outcome.GetError().GetMessage() << std::endl;
        return false;
    }

    const auto& object_list = list_objects_outcome.GetResult().GetContents();
    for (const auto& object : object_list)
    {
        Aws::S3::Model::DeleteObjectRequest delete_object_request;
        delete_object_request.WithBucket(bucket_name.c_str()).WithKey(object.GetKey());
        auto deleteOutcome = s3_client.DeleteObject(delete_object_request);

        if (!deleteOutcome.IsSuccess())
        {
            std::cerr << "Failed to delete object: " << object.GetKey() << ". Error: " << deleteOutcome.GetError().GetMessage() << std::endl;
            return false;
        }
    }

    std::cout << "All objects deleted from bucket: " << bucket_name << std::endl;
    return true;
}

bool S3Client::delete_bucket()
{
    std::string bucket_name;
    std::cout << "Enter bucket name: ";
    std::cin >> bucket_name;

    if (!delete_all_objects_in_bucket(bucket_name))
    {
        return false;
    }

    Aws::S3::Model::DeleteBucketRequest delete_bucket_request;
    delete_bucket_request.SetBucket(bucket_name.c_str());

    auto delete_bucket_outcome = s3_client.DeleteBucket(delete_bucket_request);
    if (delete_bucket_outcome.IsSuccess())
    {
        std::cout << "Successfully deleted bucket: " << bucket_name << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Failed to delete bucket: " << delete_bucket_outcome.GetError().GetMessage() << std::endl;
        return false;
    }
}


void S3Client::put_object()
{
    std::string bucket_name, object_name, file_name;
    std::cout << "Enter bucket name: ";
    std::cin >> bucket_name;
    std::cout << "Enter object name (key): ";
    std::cin >> object_name;

    // Set up the PutObject request
    Aws::S3::Model::PutObjectRequest object_request;
    object_request.SetBucket(bucket_name);
    object_request.SetKey(object_name);

    auto content = Aws::MakeShared<Aws::StringStream>("PutObjectInputStream");
    object_request.SetBody(content);

    // Upload the object
    auto put_object_outcome = s3_client.PutObject(object_request);

    if (put_object_outcome.IsSuccess())
    {
        std::cout << "Successfully created object: " << object_name << std::endl;
    }
    else
    {
        std::cerr << "Error: " << put_object_outcome.GetError().GetMessage() << std::endl;
    }
}

void S3Client::upload_file()
{
    std::string bucket_name, object_name, file_name;
    std::cout << "Enter bucket name: ";
    std::cin >> bucket_name;
    std::cout << "Enter object name (key): ";
    std::cin >> object_name;
    std::cout << "Enter file name (local path): ";
    std::cin >> file_name;

    Aws::S3::Model::PutObjectRequest request;
    request.SetBucket(bucket_name.c_str());
    request.SetKey(object_name.c_str());

    auto input_data = Aws::MakeShared<Aws::FStream>("PutObjectInputStream", file_name.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!input_data->good())
    {
        std::cerr << "Error reading file: " << file_name << std::endl;
        return;
    }
    request.SetBody(input_data);

    auto outcome = s3_client.PutObject(request);
    if (outcome.IsSuccess())
    {
        std::cout << "Successfully uploaded file to bucket: " << bucket_name << std::endl;
    }
    else
    {
        std::cerr << "Failed to upload file: " << outcome.GetError().GetMessage() << std::endl;
    }
}

void S3Client::list_objects()
{
    std::string bucket_name;
    std::cout << "Enter bucket name: ";
    std::cin >> bucket_name;

    Aws::S3::Model::ListObjectsV2Request request;
    request.SetBucket(bucket_name.c_str());

    auto outcome = s3_client.ListObjectsV2(request);
    if (outcome.IsSuccess())
    {
        std::cout << "Objects in bucket '" << bucket_name << "':" << std::endl;
        const auto& objects = outcome.GetResult().GetContents();
        for (const auto& object : objects)
        {
            std::cout << "  - " << object.GetKey() << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to list objects in bucket: " << outcome.GetError().GetMessage() << std::endl;
    }
}

void S3Client::download_file()
{
    std::string bucket_name, object_name, file_name;
    std::cout << "Enter bucket name: ";
    std::cin >> bucket_name;
    std::cout << "Enter object name (key): ";
    std::cin >> object_name;
    std::cout << "Enter file name to save to (local path): ";
    std::cin >> file_name;

    Aws::S3::Model::GetObjectRequest request;
    request.SetBucket(bucket_name.c_str());
    request.SetKey(object_name.c_str());

    auto outcome = s3_client.GetObject(request);
    if (outcome.IsSuccess())
    {
        auto& retrieved_file = outcome.GetResultWithOwnership().GetBody();
        std::ofstream output_file(file_name, std::ios::binary);
        output_file << retrieved_file.rdbuf();
        std::cout << "Successfully downloaded object: " << object_name << std::endl;
    }
    else
    {
        std::cerr << "Failed to download object: " << outcome.GetError().GetMessage() << std::endl;
    }
}

void S3Client::delete_object()
{
    std::string bucket_name, object_name;
    std::cout << "Enter bucket name: ";
    std::cin >> bucket_name;
    std::cout << "Enter object name (key): ";
    std::cin >> object_name;

    Aws::S3::Model::DeleteObjectRequest request;
    request.SetBucket(bucket_name.c_str());
    request.SetKey(object_name.c_str());

    Aws::S3::Model::HeadObjectRequest head_request;
    head_request.WithBucket(bucket_name.c_str()).WithKey(object_name.c_str());

    auto head_outcome = s3_client.HeadObject(head_request);
    if (!head_outcome.IsSuccess())
    {
        std::cerr << "Object '" << object_name << "' does not exist in bucket '" << bucket_name
                  << "' or you don't have permissions to access it." << std::endl;
        return;
    }

    auto outcome = s3_client.DeleteObject(request);
    if (outcome.IsSuccess())
    {
        std::cout << "Successfully deleted object: " << object_name << std::endl;
    }
    else
    {
        std::cerr << "Failed to delete object: " << outcome.GetError().GetMessage() << std::endl;
    }
}
