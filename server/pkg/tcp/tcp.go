package tcp

import (
	"fmt"
	"net"
)

type TCPServer struct {
	address  string
	listener net.Listener
	exit     chan struct{}
}

func CreateServer(address string) *TCPServer {
	return &TCPServer{
		address: address,
		exit:    make(chan struct{}),
	}
}

func (s *TCPServer) Start() error {
	listener, err := net.Listen("tcp", s.address)
	if err != nil {
		return err
	}
	defer listener.Close()

	fmt.Println("TCP server listening at", s.address)

	s.listener = listener

	go s.acceptConnections()

	<-s.exit

	return nil
}

func (s *TCPServer) acceptConnections() {
	for {
		conn, err := s.listener.Accept()
		if err != nil {
			fmt.Println("acceptConnections error:", err)
			continue
		}

		fmt.Println("new connection established:", conn.RemoteAddr())

		go s.handleConn(conn)
	}
}

func (s *TCPServer) handleConn(conn net.Conn) {
	defer conn.Close()
	buffer := make([]byte, 1024)

	for {
		n, err := conn.Read(buffer)
		if err != nil {
			fmt.Println("handleConn error:", err)
			continue
		}

		payload := buffer[:n]
		fmt.Println(string(payload))
	}
}
