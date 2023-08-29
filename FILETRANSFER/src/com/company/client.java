package com.company;

import java.io.*;
import java.net.Socket;

public class client {
    public static void main(String[] args) throws IOException {

        //绑定服务器端IP与端口并发送消息
        Socket socket = new Socket("8.130.115.65", 8888);
        OutputStream out = socket.getOutputStream();
        out.write("我是客户端".getBytes());
        socket.shutdownOutput();

        //输出socket服务器端返回的消息
        InputStream inputStream = socket.getInputStream();
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
        System.out.println(reader.readLine());

        new Thread(() -> {
            try (
                    //获取输入流对象
                    BufferedInputStream bis = new BufferedInputStream(socket.getInputStream());
                    //创建输出流对象, 保存到本地 .
                    FileOutputStream fis = new FileOutputStream(System.currentTimeMillis() + ".zip");
                    BufferedOutputStream bos = new BufferedOutputStream(fis);
            ) {
                //读写数据
                byte[] b = new byte[1024 * 8];
                int len;
                while ((len = bis.read(b)) != -1) {
                    bos.write(b, 0, len);
                }
                //关闭 资源
                bos.close();
                bis.close();
                socket.close();
                System.out.println("文件上传已保存");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();
    }
}
