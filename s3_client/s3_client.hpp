#pragma once

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <string>

class S3Client {
public:
    S3Client(const std::string& endpoint, const std::string& access_key, const std::string& secret_key);
    ~S3Client();

    void list_buckets();
    void create_bucket();
    bool delete_bucket();
    void upload_file();
    void list_objects();
    void download_file();
    void delete_object();
    void put_object();

private:
    Aws::S3::S3Client s3_client;
    bool delete_all_objects_in_bucket(const std::string& bucket_name);
};
