import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.ServerSocket;

public class rece {
    public static void main(String[] args) throws IOException {
        System.out.println("服务启动，等待连接中");
        //创建ServerSocket对象，绑定端口，开始等待连接
        ServerSocket ss = new ServerSocket(8888);

        //循环接收
        while (true) {
            //接受accept方法，返回socket对象
            Socket server = ss.accept();
            InetAddress address = server.getInetAddress();
            System.out.println(address);
            //开启多线程
            new Thread(() -> {
                try (
                        //获取输入流对象
                        BufferedInputStream bis = new BufferedInputStream(server.getInputStream());
                        //创建输出流对象, 保存到本地 .
                        FileOutputStream fis = new FileOutputStream(System.currentTimeMillis() + ".bin");
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
                    server.close();
                    System.out.println("文件上传已保存");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }).start();
        }
    }
}
