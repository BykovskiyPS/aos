package main


import (
	"bytes"
	"context"
	"fmt"
	"io"
	"net/http"
	"strconv"
	"strings"
	"text/template"
	"log"
	"os"
	"sync"

	tgbotapi "github.com/skinass/telegram-bot-api/v5"
)

var (
	BotToken   = "5635106517:AAFWULmeWpeiV3i4Oiurg2KHjmIP6rUnEFo"
	WebhookURL = "https://3cf5-95-220-87-9.eu.ngrok.io"
	//WebhookURL = "https://525f2cb5.ngrok.io"
)

type Task struct {
	ID         int64
	TaskName   *string
	ConsumerID *int64
	AssignerID *int64
	IsClose    bool
}

type User struct {
	ID       int64
	UserName *string
}

const (
	TaskTemplate = `{{.TaskID}}. {{.TaskName}} by @{{.AssignerName}}{{if .NeedAssignee}}
assignee: {{if .IConsumer}}я{{else}}@{{.ConsumerName}}{{end}}{{end}}{{if .NeedSuggest}}
{{if .IConsumer}}/unassign_{{.TaskID}} /resolve_{{.TaskID}}{{else}}/assign_{{.TaskID}}{{end}}{{end}}`

	NewTemplate = `Задача "{{.TaskName}}" создана, id={{.TaskID}}`

	AssignTemplate = `Задача "{{.TaskName}}" назначена на {{if .IConsumer}}вас{{else}}@{{.ConsumerName}}{{end}}`

	UnAssignTemplate = `Задача "{{.TaskName}}" осталась без исполнителя`

	ResolveTemplate = `Задача "{{.TaskName}}" выполнена{{if not .IConsumer}} @{{.ConsumerName}}{{end}}`
)


type Base struct {
	Tasks      map[int64]*Task
	Users      map[int64]*User
	Templates  map[string]*template.Template

	Handlers   map[string]func(*map[string]*string, *template.Template) (*[]SendMessage, error)
	TasksQueue []int64
	CountTask  int64
	mu         *sync.Mutex
}

type SendMessage struct {
	UserID  int64
	Message *string
}

type TaskInfo struct {
	TaskID       int64
	TaskName     string
	AssignerName string
	ConsumerName string
	IConsumer    bool
	NeedAssignee bool
	NeedSuggest  bool
}

type SelectParams struct {
	UserID       int64
	NeedAssignee bool
	NeedSuggest  bool
}

type HandleParams struct {
	Params *map[string]*string
	Name   *string
}

func (b *Base) TaskInfoSelect(TaskID int64, tmpl *template.Template, params SelectParams) (*TaskInfo, error) {
	taskInfo := TaskInfo{}
	task := b.Tasks[TaskID]
	if task == nil {
		return nil, fmt.Errorf("error: func FillTaskInfo: unknown TaskID=%d", TaskID)
	}
	taskInfo.TaskID = TaskID
	if task.TaskName == nil {
		return nil, fmt.Errorf("error: func FillTaskInfo: TaskName is nil")
	}
	taskInfo.TaskName = *task.TaskName
	if task.AssignerID == nil {
		return nil, fmt.Errorf("error: func FillTaskInfo: AssignerID is nil")
	}
	if b.Users[*task.AssignerID] == nil {
		return nil, fmt.Errorf("error: func FillTaskInfo: Unknow user with ID=%d", *task.AssignerID)
	}
	if b.Users[*task.AssignerID].UserName == nil {
		return nil, fmt.Errorf("error: func FillTaskInfo: Unknow UserName user with ID=%d", *task.AssignerID)
	}
	taskInfo.AssignerName = *b.Users[*task.AssignerID].UserName
	if task.ConsumerID == nil {
		taskInfo.NeedAssignee = false
		taskInfo.IConsumer = false
		taskInfo.NeedSuggest = params.NeedSuggest

	} else {
		ConsumerName := b.Users[*task.ConsumerID].UserName
		if ConsumerName == nil {
			return nil, fmt.Errorf("error: func FillTaskInfo: Unknow UserName UserID=%d", *task.ConsumerID)
		}
		taskInfo.NeedAssignee = params.NeedAssignee
		if *task.ConsumerID == params.UserID {
			taskInfo.IConsumer = true
			taskInfo.NeedSuggest = params.NeedSuggest
		} else {
			taskInfo.IConsumer = false
			taskInfo.ConsumerName = *ConsumerName
			taskInfo.NeedSuggest = false

		}
	}
	return &taskInfo, nil

}

func (b *Base) Select(data *[]int64, tmpl *template.Template, params SelectParams) (*[]SendMessage, error) {
	if len(*data) == 0 {
		answer := "Нет задач"
		list := []SendMessage{{
			UserID:  params.UserID,
			Message: &answer},
		}
		return &list, nil
	}
	buf := &bytes.Buffer{}
	first := (*data)[0]
	taskInfo, err := b.TaskInfoSelect(first, tmpl, params)
	if err != nil {
		return nil, err
	}
	err = tmpl.Execute(buf, taskInfo)
	if err != nil {
		return nil, err
	}

	for _, taskID := range (*data)[1:] {
		_, err := io.WriteString(buf, "\n\n")
		if err != nil {
			return nil, err
		}

		taskInfo, err := b.TaskInfoSelect(taskID, tmpl, params)
		if err != nil {
			return nil, err
		}
		err = tmpl.Execute(buf, taskInfo)
		if err != nil {
			return nil, err
		}

	}
	answer := buf.String()

	list := []SendMessage{{
		UserID:  params.UserID,
		Message: &answer},
	}
	return &list, nil
}

func GetInt64Param(params *map[string]*string, paramName string) (*int64, error) {
	if (*params)[paramName] == nil {
		return nil, fmt.Errorf("error func GetParam, parametr %s is nil ", paramName)
	}
	userID, err := strconv.Atoi(*(*params)[paramName])
	if err != nil {
		return nil, err
	}
	UserID64 := int64(userID)
	return &UserID64, nil
}

func (b *Base) TasksShow(params *map[string]*string, tmpl *template.Template) (*[]SendMessage, error) {
	list := []int64{}
	for _, taskID := range b.TasksQueue {
		if !b.Tasks[taskID].IsClose {
			list = append(list, taskID)
		}
	}
	userID, err := GetInt64Param(params, "UserID")
	if err != nil {
		return nil, err
	}
	selectParams := SelectParams{
		UserID:       *userID,
		NeedAssignee: true,
		NeedSuggest:  true,
	}
	return b.Select(&list, tmpl, selectParams)
}

func (b *Base) My(params *map[string]*string, tmpl *template.Template) (*[]SendMessage, error) {
	userID, err := GetInt64Param(params, "UserID")
	if err != nil {
		return nil, err
	}
	list := []int64{}
	for _, taskID := range b.TasksQueue {
		ConsumerID := b.Tasks[taskID].ConsumerID
		if ConsumerID == nil {
			continue
		}
		active := !b.Tasks[taskID].IsClose
		if active && *ConsumerID == *userID {
			list = append(list, taskID)
		}
	}
	selectParams := SelectParams{
		UserID:       *userID,
		NeedAssignee: false,
		NeedSuggest:  true,
	}
	return b.Select(&list, tmpl, selectParams)
}

func (b *Base) Own(params *map[string]*string, tmpl *template.Template) (*[]SendMessage, error) {
	userID, err := GetInt64Param(params, "UserID")
	if err != nil {
		return nil, err
	}
	list := []int64{}
	for _, taskID := range b.TasksQueue {
		if !b.Tasks[taskID].IsClose && *b.Tasks[taskID].AssignerID == *userID {
			list = append(list, taskID)
		}
	}
	selectParams := SelectParams{
		UserID:       *userID,
		NeedAssignee: false,
		NeedSuggest:  true,
	}
	return b.Select(&list, tmpl, selectParams)
}

func (b *Base) New(params *map[string]*string, tmpl *template.Template) (*[]SendMessage, error) {
	NewTaskId := int64(len(b.TasksQueue)) + 1
	b.mu.Lock()
	for b.Tasks[NewTaskId] != nil {
		NewTaskId += 1
	}
	b.TasksQueue = append(b.TasksQueue, NewTaskId)
	if (*params)["NewTaskName"] == nil {
		return nil, fmt.Errorf("error func New, parametr %s is nil ", "NewTaskName")
	}
	taskName := (*params)["NewTaskName"]
	AssignerID, err := GetInt64Param(params, "UserID")
	if err != nil {
		return nil, err
	}
	task := Task{
		ID:         NewTaskId,
		TaskName:   taskName,
		ConsumerID: nil,
		AssignerID: AssignerID,
		IsClose:    false,
	}
	b.Tasks[NewTaskId] = &task
	b.mu.Unlock()
	taskInfo := TaskInfo{
		TaskID:   NewTaskId,
		TaskName: *taskName,
	}
	buf := &bytes.Buffer{}
	err = tmpl.Execute(buf, taskInfo)
	if err != nil {
		return nil, err
	}
	answer := buf.String()
	list := []SendMessage{{
		UserID:  *AssignerID,
		Message: &answer},
	}
	return &list, nil
}

func (b *Base) Assign(params *map[string]*string, tmpl *template.Template) (*[]SendMessage, error) {
	UserID, err := GetInt64Param(params, "UserID")
	if err != nil {
		return nil, err
	}
	TaskID, err := GetInt64Param(params, "TaskID")
	if err != nil {
		return nil, err
	}
	task := b.Tasks[*TaskID]
	if task == nil {
		return nil, err
	}
	taskInfo1 := TaskInfo{
		TaskName:  *task.TaskName,
		IConsumer: true,
	}
	buf1 := &bytes.Buffer{}
	err = tmpl.Execute(buf1, taskInfo1)
	if err != nil {
		return nil, err
	}
	answer1 := buf1.String()
	list := []SendMessage{}
	list = append(list, SendMessage{
		UserID:  *UserID,
		Message: &answer1,
	})
	PrevUserID := task.ConsumerID
	if PrevUserID != nil {
		taskInfo2 := TaskInfo{
			TaskName:     *task.TaskName,
			IConsumer:    false,
			ConsumerName: *b.Users[*UserID].UserName,
		}
		buf2 := &bytes.Buffer{}
		err = tmpl.Execute(buf2, taskInfo2)
		if err != nil {
			return nil, err
		}
		answer2 := buf2.String()
		list = append(list, SendMessage{
			UserID:  *PrevUserID,
			Message: &answer2,
		})
	} else {
		if *task.AssignerID != *UserID {
			taskInfo2 := TaskInfo{
				TaskName:     *task.TaskName,
				IConsumer:    false,
				ConsumerName: *b.Users[*UserID].UserName,
			}
			buf2 := &bytes.Buffer{}
			err = tmpl.Execute(buf2, taskInfo2)
			if err != nil {
				return nil, err
			}
			answer2 := buf2.String()
			list = append(list, SendMessage{
				UserID:  *task.AssignerID,
				Message: &answer2,
			})
		}
	}
	b.mu.Lock()
	task.ConsumerID = UserID
	b.mu.Unlock()
	return &list, nil
}

func (b *Base) UnAssign(params *map[string]*string, tmpl *template.Template) (*[]SendMessage, error) {
	UserID, err := GetInt64Param(params, "UserID")
	if err != nil {
		return nil, err
	}
	TaskID, err := GetInt64Param(params, "TaskID")
	if err != nil {
		return nil, err
	}
	task := b.Tasks[*TaskID]
	if *UserID != *task.ConsumerID {
		answer := `Задача не на вас`
		list := []SendMessage{{
			UserID:  *UserID,
			Message: &answer},
		}
		return &list, nil
	}
	answer1 := `Принято`
	list := []SendMessage{{
		UserID:  *UserID,
		Message: &answer1},
	}
	taskInfo := TaskInfo{
		TaskName: *task.TaskName,
	}
	buf := &bytes.Buffer{}
	err = tmpl.Execute(buf, taskInfo)
	if err != nil {
		return nil, err
	}
	answer2 := buf.String()
	list = append(list, SendMessage{
		UserID:  *task.AssignerID,
		Message: &answer2,
	})
	b.mu.Lock()
	task.ConsumerID = nil
	b.mu.Unlock()
	return &list, nil
}

func (b *Base) Resolve(params *map[string]*string, tmpl *template.Template) (*[]SendMessage, error) {
	UserID, err := GetInt64Param(params, "UserID")
	if err != nil {
		return nil, err
	}
	TaskID, err := GetInt64Param(params, "TaskID")
	if err != nil {
		return nil, err
	}
	task := b.Tasks[*TaskID]
	taskInfo := TaskInfo{
		TaskName:  *task.TaskName,
		IConsumer: true,
	}
	buf := &bytes.Buffer{}
	err = tmpl.Execute(buf, taskInfo)
	if err != nil {
		return nil, err
	}
	answer := buf.String()
	list := []SendMessage{{
		UserID:  *UserID,
		Message: &answer,
	},
	}
	if *task.AssignerID != *UserID {
		taskInfo2 := TaskInfo{
			IConsumer:    false,
			ConsumerName: *b.Users[*UserID].UserName,
			TaskName:     *task.TaskName,
		}
		buf2 := &bytes.Buffer{}
		err2 := tmpl.Execute(buf2, taskInfo2)
		if err2 != nil {
			return nil, err2
		}
		answer2 := buf2.String()
		Message2 := SendMessage{
			UserID:  *task.AssignerID,
			Message: &answer2,
		}
		list = append(list, Message2)
	}

	b.mu.Lock()
	task.IsClose = true
	b.mu.Unlock()
	return &list, nil
}

func (b *Base) CheckHandler(HandlerName *string) error {
	if HandlerName == nil {
		return fmt.Errorf("error HandlerName is nil")
	}
	if b.Handlers[*HandlerName] == nil {
		return fmt.Errorf("error Unknow Handler")
	}
	if b.Templates[*HandlerName] == nil {
		return fmt.Errorf("error Unknow Template")
	}
	return nil
}

func GetTemplate(TemplateName string, Message string) (*template.Template, error) {
	tmpl := template.New(TemplateName)
	tmpl, err := tmpl.Parse(Message)
	if err != nil {
		log.Fatalln("error Template: ", err)
		return nil, err
	}
	return tmpl, nil
}

func (b *Base) InitTemplates() error {
	if b.Templates == nil {
		b.Templates = map[string]*template.Template{}
	}
	Messages := []string{
		TaskTemplate,
		NewTemplate,
		AssignTemplate,
		UnAssignTemplate,
		TaskTemplate,
		TaskTemplate,
		ResolveTemplate,
	}
	TemplateNames := []string{
		"/tasks",
		"/new",
		"/assign",
		"/unassign",
		"/my",
		"/owner",
		"/resolve",
	}
	

	for i := range Messages {
		tmpl, err := GetTemplate(TemplateNames[i], Messages[i])
		if err != nil {
			return err
		}
		b.Templates[TemplateNames[i]] = tmpl
	}
	return nil
}

func (b *Base) InitHandlers() error {
	if b.Handlers == nil {
		b.Handlers = map[string]func(*map[string]*string, *template.Template) (*[]SendMessage, error){}
	}
	b.Handlers["/tasks"] = b.TasksShow
	b.Handlers["/new"] = b.New
	b.Handlers["/assign"] = b.Assign
	b.Handlers["/unassign"] = b.UnAssign
	b.Handlers["/resolve"] = b.Resolve
	b.Handlers["/my"] = b.My
	b.Handlers["/owner"] = b.Own
	return nil
}

func (b *Base) InitBase() error {
	err := b.InitTemplates()
	if err != nil {
		return err
	}
	err = b.InitHandlers()
	if err != nil {
		return err
	}
	b.Users = map[int64]*User{}
	b.Tasks = map[int64]*Task{}
	b.mu = &sync.Mutex{}
	return nil
}

func ParseText(str string) (*HandleParams, error) {
	str1 := strings.Replace(str, "_", " ", 1)
	strs := strings.SplitN(str1, " ", 2)
	data := map[string]*string{}
	data["HandlerName"] = &strs[0]
	if len(strs) == 1 {
		return &HandleParams{
			Params: &data,
			Name:   &strs[0],
		}, nil
	}

	if len(strs) == 2 {
		switch strs[0] {
		case "/new":
			data["NewTaskName"] = &strs[1]
			return &HandleParams{
				Params: &data,
				Name:   &strs[0],
			}, nil
		case "/assign":
			data["TaskID"] = &strs[1]
			return &HandleParams{
				Params: &data,
				Name:   &strs[0],
			}, nil
		case "/unassign":
			data["TaskID"] = &strs[1]
			return &HandleParams{
				Params: &data,
				Name:   &strs[0],
			}, nil
		case "/resolve":
			data["TaskID"] = &strs[1]
			return &HandleParams{
				Params: &data,
				Name:   &strs[0],
			}, nil
		default:
			return nil, fmt.Errorf("error Incorrect parsing message")
		}
	}
	return nil, fmt.Errorf("error Incorect command length")
}

func (b *Base) MessageProcessing(Message *tgbotapi.Message) (*HandleParams, error) {
	UserID := Message.From.ID
	if b.Users[UserID] == nil {
		b.Users[UserID] = &User{
			ID:       UserID,
			UserName: &Message.From.UserName,
		}
	}
	handleParams, err := ParseText(Message.Text)
	if err != nil {
		return nil, err
	}
	StrUserID := strconv.Itoa(int(UserID))
	(*handleParams.Params)["UserID"] = &StrUserID
	err = b.CheckHandler(handleParams.Name)
	if err != nil {
		return nil, err
	}
	return handleParams, nil
}

func startTaskBot(ctx context.Context) error {

	b := Base{
		Users:     map[int64]*User{},
		Tasks:     map[int64]*Task{},
		Templates: map[string]*template.Template{},
	}

	err := b.InitBase()
	if err != nil {
		return err
	}

	bot, err := tgbotapi.NewBotAPI(BotToken)
	if err != nil {
		log.Fatalln(err)
		return err
	}

	wh, err := tgbotapi.NewWebhook(WebhookURL)
	if err != nil {
		log.Fatalln(err)
		return err
	}

	_, err = bot.Request(wh)
	if err != nil {
		log.Fatalln(err)
		return err
	}
	updates := bot.ListenForWebhook("/")
	port := os.Getenv("PORT")
	if port == "" {
		port = "8081"
	}
	go func() {
		log.Fatalln("http err:", http.ListenAndServe(":"+port, nil))
	}()
	for update := range updates {
		handleParams, err := b.MessageProcessing(update.Message)
		if err != nil {
			log.Fatalln(err)
			continue
		}
		list, err := b.Handlers[*handleParams.Name](handleParams.Params, b.Templates[*handleParams.Name])
		if err != nil {
			log.Fatalln(err)
			continue
		}
		for _, item := range *list {
			bot.Send(tgbotapi.NewMessage(
				item.UserID,
				*item.Message,
			))
		}
	}
}

func main() {
	err := startTaskBot(context.Background())
	if err != nil {
		panic(err)
	}
}
