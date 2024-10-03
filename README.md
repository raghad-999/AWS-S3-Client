# S3 Client Application
This project implements a simple client for interacting with an S3-compatible object storage service, such as MinIO or AWS S3. The client allows you to perform common S3 operations like listing buckets, uploading files, downloading objects, and deleting buckets or objects.

# Features
- List all buckets
- Create a new bucket
- Delete a bucket (with optional removal of all objects)
- List objects in a bucket
- Upload a file to a bucket
- Download an object from a bucket
- Delete an object from a bucket
- Create (put) an object

# Prerequisites
- C++ compiler that supports C++11 standard
- AWS SDK for C++ installed
- AWS SDK core and S3 libraries
- If you're using MinIO or other S3-compatible services, make sure you provide the correct endpoint and credentials in the code.
