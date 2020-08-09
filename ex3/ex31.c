
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

void writeFileToBuffer(char* buffer, int fd) {
    char ch;
    int i = 0;
    int j = 0;

    lseek(fd, 0, SEEK_SET);

    while (read(fd, &ch, 1) > 0) {
        buffer[i] = ch;
        ++i;
    }
    buffer[i] = '\0';
}

int checkFilesSimilarity(int lengthBuffer1, int lengthBuffer2, int countMatches){
    int minLength = lengthBuffer1>lengthBuffer2 ? lengthBuffer2:lengthBuffer1;
    int checkSimilarity = countMatches>minLength/2?3:0;
    return  checkSimilarity;
}

int checkBuffers(const char *buffer1, const char *buffer2, int lengthBuffer1, int lengthBuffer2) {
    int i =0, j,k, counter;
    char ch_buf1, ch_buf2;
    int lengthEqueal = lengthBuffer1 == lengthBuffer2;

    //loop and check if the files are alike or different (for the buffers without new lines and spaces)
    while (buffer1[i] != '\0') {
        j=0;
        counter = 0;
        k=i;
        while((ch_buf2=buffer2[j])!='\0'){
            ch_buf1= buffer1[k];
            if(ch_buf1=='\0'){ break;}
            counter+=ch_buf1==ch_buf2;
            k++;
            j++;
        }

        if (lengthEqueal && counter == lengthBuffer1){
            return 1; // identical files
        }
        if ( checkFilesSimilarity(lengthBuffer1,lengthBuffer2,counter) == 3){
            return 3; // similar files
        }
        ++i;
    }

    return 2; // not the same
}

int filesIdentical(int fd1, int fd2) {
    //find length of file
    int length_f1 = lseek(fd1, 0, SEEK_END);
    int length_f2 = lseek(fd2, 0, SEEK_END);

    // both of the files are empty => identical
    if (length_f1 == 0 && length_f2 ==0){
        return 1;
    }

    char* errorMsg =  "the was a problem in the LSEEK action \n";

    if(length_f1 < 0|| length_f2 < 0) {
        write(3,errorMsg,strlen(errorMsg));
        return -1;
    }

    // moving the pointer to the start
    lseek(fd1,0,SEEK_SET);
    lseek(fd2,0,SEEK_SET);

    //copy entire file to buffer
    char buffer_f1[length_f1];
    char buffer_f2[length_f2];
    writeFileToBuffer(buffer_f1, fd1);
    writeFileToBuffer(buffer_f2, fd2);

    int val = checkBuffers(buffer_f1, buffer_f2, length_f1, length_f2);
    if(val == 2){
        val = checkBuffers(buffer_f2, buffer_f1, length_f2, length_f1);
    }
    return val;
}

int main(int argc, char* argv[]) {
    //open files in reading mode
    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_RDONLY);

    //check if file pointers were null
    if (fd1 < 0 || fd2 < 0)
    {
        char* errorOpenMsg ="Error while opening the file.\n";
        write(3,errorOpenMsg,strlen(errorOpenMsg));
        return -1;
    }

    int ret = filesIdentical(fd1, fd2);

    //close the files
    if ( close(fd1) <0 || close(fd2)<0){
        char* errorCloseMsg ="Error while closing the file.\n";
        write(3,errorCloseMsg,strlen(errorCloseMsg));
        return -1;
    }

    return ret;
}
