module vulnerable-go-app

go 1.16

require (
	// VULNERABILITY: Outdated dependencies with known CVEs
	github.com/gin-gonic/gin v1.6.3
	github.com/go-sql-driver/mysql v1.5.0
	github.com/gorilla/mux v1.8.0
	github.com/gorilla/sessions v1.2.1
	github.com/gorilla/websocket v1.4.2
	github.com/lib/pq v1.8.0
	github.com/mattn/go-sqlite3 v1.14.6
	github.com/golang-jwt/jwt v3.2.2+incompatible
	github.com/golang/protobuf v1.4.3
	github.com/google/uuid v1.1.2
	github.com/hashicorp/consul v1.8.4
	github.com/hashicorp/vault v1.4.1
	github.com/influxdata/influxdb v1.8.4
	github.com/jinzhu/gorm v1.9.16
	github.com/kataras/iris/v12 v12.1.8
	github.com/labstack/echo/v4 v4.1.17
	github.com/micro/go-micro v1.18.0
	github.com/nats-io/nats.go v1.10.0
	github.com/nsqio/go-nsq v1.0.8
	github.com/olivere/elastic v6.2.35+incompatible
	github.com/patrickmn/go-cache v2.1.0+incompatible
	github.com/prometheus/client_golang v1.8.0
	github.com/redis/go-redis/v8 v8.4.0
	github.com/sirupsen/logrus v1.7.0
	github.com/spf13/cobra v1.1.1
	github.com/spf13/viper v1.7.1
	github.com/stretchr/testify v1.6.1
	github.com/valyala/fasthttp v1.16.0
	golang.org/x/crypto v0.0.0-20201221181555-eec23a3978ad
	golang.org/x/net v0.0.0-20201224014010-6772e930b67b
	golang.org/x/oauth2 v0.0.0-20201208152858-08078c50e5b5
	golang.org/x/sys v0.0.0-20210113181707-4bcb84eeeb78
	golang.org/x/text v0.3.4
	google.golang.org/grpc v1.34.0
	google.golang.org/protobuf v1.25.0
	gopkg.in/yaml.v2 v2.4.0
	gopkg.in/yaml.v3 v3.0.0-20200615113413-eeeca48fe776
	k8s.io/api v0.20.2
	k8s.io/apimachinery v0.20.2
	k8s.io/client-go v0.20.2
)
