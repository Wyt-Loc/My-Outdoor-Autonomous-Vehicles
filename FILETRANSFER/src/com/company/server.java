package com.company;


import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

public class server {
    public static void main(String[] args) throws IOException {

        //设定服务器端口，ip不设置表示默认的本机ip
        ServerSocket server = new ServerSocket(8888);
        while (true) {
            Socket clientSocket = server.accept();

            BufferedReader br = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            String msg = br.readLine();

            //输出socket客户端发送进来的消息
            System.out.println(msg);
            clientSocket.shutdownInput();

            //返回消息给socket客户端
            PrintWriter writer = new PrintWriter(clientSocket.getOutputStream());
            writer.println(clientSocket.getPort() + "服务器返回：" + msg);
            writer.flush();

            BufferedInputStream bis = new BufferedInputStream(new FileInputStream("0.05GB.bin"));
            BufferedOutputStream bos = new BufferedOutputStream(clientSocket.getOutputStream());
            //写出数据
            byte[] b = new byte[1024 * 8];
            int len;
            while ((len = bis.read(b)) != -1) {
                bos.write(b, 0, len);
                bos.flush();
            }
            System.out.println("文件已上传");

            //收到此消息退出socket服务器端
            if (msg.equals("exit")) {
                server.close();
            }
            bis.close();
            bos.close();
        }

    }
}
