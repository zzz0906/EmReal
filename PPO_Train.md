# PPO->Train Process
## Q1 How does the training process interacte with the envs?
In train.py we use a code
```
batch_env = utility.define_batch_env(lambda: _create_environment(config), config.num_agents,
                                         env_processes)
```
```
if env_processes:
      envs = [wrappers.ExternalProcess(constructor) for _ in range(num_agents)]
```
we can see that the envs use wrappers.ExternalProcess so that we can find the code
it define the basic envs' parameters like the following:
```
def step(self, action, blocking=True):
    """Step the environment.

    Args:
      action: The action to apply to the environment.
      blocking: Whether to wait for the result.

    Returns:
      Transition tuple when blocking, otherwise callable that returns the
      transition tuple.
    """
    self._conn.send((self._ACTION, action))
    if blocking:
      return self._receive(self._TRANSITION)
    else:
      return functools.partial(self._receive, self._TRANSITION)
```
The program use a multi process to manage the action
Then we need to know how they get the action from the bullet spaces. 

SO! we can know that the real variable use a multi processes to pass the variable.

So the real environment is we pass through another process. Where is it?
Train->define_batch_env->wrappers->constructor->openaigym->lambda->env = gym.make(config.env)

Oh, it turns out to be that. we just define a minitaur environment extends from the gym environment.