diff --git a/exemplo/main.c b/exemplo/main.c
index f8b96df..646d0d0 100755
--- a/exemplo/main.c
+++ b/exemplo/main.c
@@ -518,8 +518,10 @@ int main()
     //DIRENT2 dirent[50];
     //readdir2(dirHandle, dirent);
     //printf("OOk");
-
-    char cmd[256];
+
+    fileHandle = open2("./dir1/../dir1/file1.txt");
+    printf("ok2");
+   /* char cmd[256];
     char *token;
     int i,n;
     int flagAchou, flagEncerrar;
@@ -556,7 +558,7 @@ int main()
 			if (!flagAchou) printf ("???\n");
         }
 		if (flagEncerrar) break;
-    }
+    } */
     return 0;
 }
 
diff --git a/include/config.h b/include/config.h
index f116b93..81e4e92 100644
--- a/include/config.h
+++ b/include/config.h
@@ -5,7 +5,7 @@
 #ifndef __cconfig__
 #define __cconfig__
 
-// ERROR CODES
+// ERROR CODES 
 #define FUNC_WORKING 0
 #define VALID_TYPE 0
 #define FUNC_NOT_IMPLEMENTED -1
@@ -21,8 +21,6 @@
 #define WRITE_ERROR -10
 #define CH_ERROR -11
 #define INVALID_LINK_TYPE -12
-#define SEEK_ERROR -13;
-#define TRUNCATE_ERROR -14;
 
 #define MAX_OPEN_FILES 10
 
diff --git a/src/t2fs.c b/src/t2fs.c
index 61e66c4..05cb501 100644
--- a/src/t2fs.c
+++ b/src/t2fs.c
@@ -423,25 +423,7 @@ Saída:	Se a operação foi realizada com sucesso, a função retorna "0" (zero)
 int seek2 (FILE2 handle, DWORD offset) {
   initT2fs();
 
-  if(opened_files_map[handle] == 0){
-    printf("Error File not open!");
-    return SEEK_ERROR;
-  }
-
-  Record *rec = opened_files[handle].frecord;
-
-  if((int)offset > (int)rec->bytesFileSize || (int)offset < -1){
-    printf("To big or to small! offset:%d\nsize:%d",offset,rec->bytesFileSize);
-    return SEEK_ERROR;
-  }
-
-  if(offset == -1){
-    opened_files[handle].curr_pointer += rec->bytesFileSize;
-  }
-  else{
-    opened_files[handle].curr_pointer = offset;
-  }
-  return FUNC_WORKING;
+  return FUNC_NOT_WORKING;
 }
 
 
@@ -527,7 +509,7 @@ int mkdir2 (char *pathname) {
   // points to the parent's firstCluster
   parent.firstCluster = dir[0].firstCluster;
 
-  Record *newDir = malloc(clusterSize);
+  Record *newDir = malloc(clusterSize); 
   newDir[0] = this;
   newDir[1] = parent;
 
