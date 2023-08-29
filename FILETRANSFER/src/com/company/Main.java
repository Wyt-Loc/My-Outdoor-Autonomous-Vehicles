package com.company;

import java.io.*;
import java.net.Socket;


public class Main {

    public static void main(String[] args) throws IOException {
        //创建输入流
        BufferedInputStream bis = new BufferedInputStream(new FileInputStream("0.05GB.bin"));
        //创建Socket
        Socket client = new Socket("8.130.115.65", 8888);
        //输出流
        BufferedOutputStream bos = new BufferedOutputStream(client.getOutputStream());
        //写出数据
        byte[] b = new byte[1024 * 8];
        int len;
        while ((len = bis.read(b)) != -1) {
            bos.write(b, 0, len);
            bos.flush();
        }
        System.out.println("文件已上传");

        //关闭资源
        bos.close();
        bis.close();
        client.close();
        System.out.println("文件上传完成");

    }
}
