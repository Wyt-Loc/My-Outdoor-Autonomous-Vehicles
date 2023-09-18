package com.example.remoteapp;

import static java.lang.Integer.parseInt;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;


public class MainActivity extends AppCompatActivity {

    Button button_con, button_disco, button_send, button_go, button_back, button_left, button_right, button_stop;
    TextView textView_icon, textView_ip, textView_com, textView_info;

    String ip, com_s;
    int com;
    Socket socket = null;

    Mysocket client;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // 按钮
        button_con = findViewById(R.id.button_con);
        button_disco = findViewById(R.id.button_disco);
        button_send = findViewById(R.id.button_send);
        button_go = findViewById(R.id.button_go);
        button_back = findViewById(R.id.button_back);
        button_left = findViewById(R.id.button_left);
        button_right = findViewById(R.id.button_right);
        button_stop = findViewById(R.id.button_stop);

        // txt
        textView_icon = findViewById(R.id.textView_isconn);
        textView_com = findViewById(R.id.textView_com);
        textView_info = findViewById(R.id.textView_info);
        textView_ip = findViewById(R.id.textView_ip);


        // 连接按钮逻辑
        button_con.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                ip = textView_ip.getText().toString();
                com_s = textView_com.getText().toString();
                if ("".equals(ip) || "".equals(com_s)) {
                    Log.i("main", "onClick: 111");
                    Toast.makeText(MainActivity.this, "自动连接默认IP和端口", Toast.LENGTH_SHORT).show();
                    ip = "8.130.115.65";
                    com = 8887;
                    client = new Mysocket();
                    client.start();
                } else {
                    com = Integer.parseInt(com_s);
                    client = new Mysocket();
                    client.start();
                }
            }
        });

        // 断开连接的逻辑
        button_disco.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    socket.close();
                    Toast.makeText(MainActivity.this, "断开连接", Toast.LENGTH_SHORT).show();
                } catch (IOException e) {
                    Toast.makeText(MainActivity.this, "未发现连接", Toast.LENGTH_SHORT).show();
                    e.printStackTrace();
                }
            }
        });

        // 发送数据逻辑
        button_send.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //子线程中进行网络操作
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (socket != null) {
                            try {
                                String text = textView_info.getText().toString();
                                client.outputStream.write(text.getBytes());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        } else {
                            runOnUiThread(new Runnable()//不允许其他线程直接操作组件，用提供的此方法可以
                            {
                                public void run() {
                                    Toast.makeText(MainActivity.this, "请先建立连接", Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                    }
                }).start();
            }
        });


        // 前进按键底
        button_go.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //子线程中进行网络操作
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (socket != null) {
                            try {
                                String text = "advancesl";
                                client.outputStream.write(text.getBytes());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        } else {
                            runOnUiThread(new Runnable()//不允许其他线程直接操作组件，用提供的此方法可以
                            {
                                public void run() {
                                    Toast.makeText(MainActivity.this, "请先建立连接", Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                    }
                }).start();
            }
        });

        // 后退
        button_back.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //子线程中进行网络操作
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (socket != null) {
                            try {
                                String text = "backl";
                                client.outputStream.write(text.getBytes());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        } else {
                            runOnUiThread(new Runnable()//不允许其他线程直接操作组件，用提供的此方法可以
                            {
                                public void run() {
                                    Toast.makeText(MainActivity.this, "请先建立连接", Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                    }
                }).start();
            }
        });

        // 前进按键底
        button_left.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //子线程中进行网络操作
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (socket != null) {
                            try {
                                String text = "lefts";
                                client.outputStream.write(text.getBytes());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        } else {
                            runOnUiThread(new Runnable()//不允许其他线程直接操作组件，用提供的此方法可以
                            {
                                public void run() {
                                    Toast.makeText(MainActivity.this, "请先建立连接", Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                    }
                }).start();
            }
        });

        // 右转按键底
        button_right.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //子线程中进行网络操作
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (socket != null) {
                            try {
                                String text = "rights";
                                client.outputStream.write(text.getBytes());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        } else {
                            runOnUiThread(new Runnable()//不允许其他线程直接操作组件，用提供的此方法可以
                            {
                                public void run() {
                                    Toast.makeText(MainActivity.this, "请先建立连接", Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                    }
                }).start();
            }
        });

        // 右转按键底
        button_stop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //子线程中进行网络操作
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if (socket != null) {
                            try {
                                String text = "stop";
                                client.outputStream.write(text.getBytes());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        } else {
                            runOnUiThread(new Runnable()//不允许其他线程直接操作组件，用提供的此方法可以
                            {
                                public void run() {
                                    Toast.makeText(MainActivity.this, "请先建立连接", Toast.LENGTH_SHORT).show();
                                }
                            });
                        }
                    }
                }).start();
            }
        });
    }

    class Mysocket extends Thread {

        OutputStream outputStream = null;
        InputStream inputStream = null;

        @Override
        public void run() {
            try {
                socket = new Socket(ip, com);
                runOnUiThread(new Runnable()//不允许其他线程直接操作组件，用提供的此方法可以
                {
                    public void run() {
                        // TODO Auto-generated method stub
                        Toast.makeText(MainActivity.this, "连接成功", Toast.LENGTH_SHORT).show();
                    }
                });
            } catch (IOException e) {
                e.printStackTrace();
            }


            if (socket != null) {
                //获取输出流对象
                try {
                    outputStream = socket.getOutputStream();
                    outputStream.write(123);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                try {
                    do {
                        final byte[] buffer = new byte[1024];//创建接收缓冲区
                        inputStream = socket.getInputStream();
                        final int len = inputStream.read(buffer);//数据读出来，并且返回数据的长度
                        runOnUiThread(new Runnable()//不允许其他线程直接操作组件，用提供的此方法可以
                        {
                            public void run() {
                                Toast.makeText(MainActivity.this, "zixianc", Toast.LENGTH_SHORT).show();
                            }
                        });
                    } while (true);
                } catch (IOException ignored) {

                }
            }
        }
    }

}
