workspace {

    model {
        user = person "User"
        game = softwareSystem "TypeTank game" {
            client = container "Game client" {
                graphics = component "Graphics"
                c_net = component "Communication"
                c_logic = component "ScriptParser"
                c_file_io = component "File IO"
                c_ecs = component "EntityComponentSystem"
            }
            server = container "Game server" {
                s_net = component "Communication"
                s_logic = component "Game logic"
                s_ecs = component "EntityComponentSystem"
            }
        }
        txt_editor = softwareSystem "Text editor" {
            tags "External Software System"
        }

        user -> client "Views"
        user -> txt_editor "Modifies script file"
        txt_editor -> client "Gives instructions via saved script file"
        client -> server "Communicates with"
        c_ecs -> c_net
        c_ecs -> graphics
        c_ecs -> c_logic
        c_logic -> c_file_io

        s_ecs -> s_logic
        s_ecs -> s_net
    }

    views {
        theme default
        styles {
            element "External Software System" {
                background #aaaaaa
                shape RoundedBox
            }
        }

        systemContext game "Diagram1" {
            include *
            autoLayout lr
        }

        container game {
            include user txt_editor client server
            autoLayout lr
        }

        component client {
            include *
            autoLayout lr
        }

        component server {
            include *
            autoLayout lr
        }
}