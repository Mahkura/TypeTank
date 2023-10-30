package main

import (
	"log"

	"github.com/Mahkura/TypeTank/server/pkg/tcp"
)

func main() {
	server := tcp.CreateServer(":8080")
	log.Fatal(server.Start())
}
