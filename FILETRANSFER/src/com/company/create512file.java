package com.company;


import java.io.*;

public class create512file {

    private static void writeFile(float N) {
        if (N < 1) {
            System.out.println("writeFile please input one integer greater than 0");
           // return;
        }

        System.out.println("writeFile start >>>");
        RandomAccessFile file = null;
        try {
            System.out.println("Will write " + N + "GB data ...");
            final String FILE_NAME = N + "GB.bin";
            File f = new File(FILE_NAME);
            if (f.exists()) {
                f.delete();
            }
            file = new RandomAccessFile(FILE_NAME, "rw");
            final int ONE_G_LENGTH = 1073741824; // 1G
            byte[] b = new byte[ONE_G_LENGTH]; // 1 GB
            b[0] = '0';
            b[b.length - 1] = '1';
            for (int i = 0; i < N; ++i) {
                file.write(b);
                file.seek(file.length());
                System.out.println("write " + (i + 1) + "GB data");
            }
            file.close();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } finally {
            if (file != null) {
                try {
                    file.close();
                    file = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        System.out.println("writeFile end <<<");
    }

    public static void main(String[] args) {
        writeFile(0.05f);
    }
}
