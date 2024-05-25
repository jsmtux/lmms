package com.jsmtux.lmms;

import org.libsdl.app.SDLActivity;
import org.libsdl.app.SDLAudioManager;
import org.libsdl.app.SDL;
import android.app.AlertDialog;
import android.os.Bundle;
import android.os.Environment;
import android.content.Intent;
import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;
import java.io.*; 

public class LMMSActivity extends org.qtproject.qt.android.bindings.QtActivity
{
        @Override
        public void onCreate( Bundle savedInstanceState )
        {
	        Log.d("LMMSActivity", "onCreate");
			File lmms_files_dir = new File(getApplicationInfo().dataDir, "files");
			lmms_files_dir.mkdir();
			listAssetFiles("lmms", lmms_files_dir);

			// SDL.loadLibrary("SDL2");
			// SDLAudioManager.setContext(null);
			// SDLActivity.nativeSetupJNI();
			// SDLAudioManager.nativeSetupJNI();
			// SDLAudioManager.initialize();
			// SDLAudioManager.setContext(this);
			super.onCreate( savedInstanceState );
        }
        @Override
        protected void onDestroy()
        {
                super.onDestroy();
		Log.d("LMMSActivity", "onDestroy");
        }

		public void listAssetFiles(String path, File parent){
			String [] list;

			try {
				list = getAssets().list(path);
				if(list.length > 0){
					for(String file : list){
						System.out.println("File path = "+file);

						if(file.indexOf(".") < 0) { // <<-- check if filename has a . then it is a file - hopefully directory names dont have . 
							System.out.println("This is a folder = "+path+"/"+file);
							if(path.equals("")) {
								listAssetFiles(file, parent); // <<-- To get subdirectory files and directories list and check 
							}else{
								File subdir = new File(parent, file);
								subdir.mkdir();
								listAssetFiles(path+"/"+file, subdir); // <<-- For Multiple level subdirectories
							}
						}else{
							System.out.println("This is a file = "+path+"/"+file);
							// copyAssetfile(getApplicationContext(), path+file, path+file);
							InputStream in = getAssets().open(path+"/"+file);
							File outFile = new File(parent, file);
							OutputStream out = new FileOutputStream(outFile);
							System.out.println("Writing to: " + outFile.getPath());

							copyFile(in, out);

							in.close();
							out.flush();
							out.close();
						}
					}

				}else{
					System.out.println("Failed Path = "+path);
					System.out.println("Check path again.");
				}
			}catch(IOException e){
				e.printStackTrace();
			}
		}

		public void copyFile(InputStream in, OutputStream out) throws IOException {
			byte[] buffer = new byte[1024];
			int read;
			while((read = in.read(buffer)) != -1){
				out.write(buffer, 0, read);
			}
		}

		// public void copyAssetFile(Context context, String assetFileName, String destinationFileName) throws IOException {
		// 	AssetManager assetManager = context.getAssets();


		// }

}